#include "GameEngine.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

GameEngine::GameEngine(WordManager& wordManager)
    : m_wordManager(wordManager), m_visibleWidth(100), m_visibleHeight(GameConfig::GAME_AREA_HEIGHT), m_gen(m_rd())
{}

void GameEngine::start(int screenWidth)
{
    updateVisibleArea(screenWidth, GameConfig::GAME_AREA_HEIGHT);
    m_isRunning = true;
    m_isPaused = false;
    m_startTime = std::chrono::steady_clock::now();
    m_totalPausedTime = 0.0f;

    // Reset difficulty
    m_currentTeleportInterval = GameConfig::BASE_TELEPORT_INTERVAL;
    m_currentSpawnIntervalMin = GameConfig::SPAWN_INTERVAL_MIN;
    m_currentSpawnIntervalMax = GameConfig::SPAWN_INTERVAL_MAX;

    // Reset state
    m_fallingWords.clear();
    m_currentInput.clear();
    m_stats = GameStats();
    m_stats.health = GameConfig::MAX_HEALTH;

    m_nextSpawnTime = getRandomSpawnInterval();
    m_flashRedBorder = false;
}

void GameEngine::update(float deltaTime)
{
    if (!m_isRunning || m_isPaused)
        return;

    // Update difficulty (teleport interval decreases over time)
    updateDifficulty(deltaTime);

    // Update falling words
    updateFallingWords(deltaTime);

    // Check if we need to spawn a new word
    m_nextSpawnTime -= deltaTime;
    if (m_nextSpawnTime <= 0.0f && m_fallingWords.size() < GameConfig::MAX_CONCURRENT_WORDS)
    {
        spawnWord();
        m_nextSpawnTime = getRandomSpawnInterval();
    }

    // Update red border flash effect
    if (m_flashRedBorder)
    {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration<float>(now - m_flashStartTime).count();
        if (elapsed >= GameConfig::BORDER_FLASH_DURATION)
        {
            m_flashRedBorder = false;
        }
    }
}

void GameEngine::pause()
{
    if (m_isRunning && !m_isPaused)
    {
        m_isPaused = true;
        m_pauseTime = std::chrono::steady_clock::now();
    }
}

void GameEngine::resume()
{
    if (m_isRunning && m_isPaused)
    {
        auto now = std::chrono::steady_clock::now();
        m_totalPausedTime += std::chrono::duration<float>(now - m_pauseTime).count();
        m_isPaused = false;
    }
}

void GameEngine::stop()
{
    m_isRunning = false;
}

void GameEngine::handleCharInput(char c)
{
    if (!m_isRunning || m_isPaused)
        return;

    // Only accept letters
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
    {
        m_currentInput += static_cast<char>(std::tolower(c));
    }
}

void GameEngine::handleBackspace()
{
    if (!m_isRunning || m_isPaused)
        return;

    if (!m_currentInput.empty())
    {
        m_currentInput.pop_back();
    }
}

void GameEngine::handleSpace()
{
    if (!m_isRunning || m_isPaused)
        return;
    if (m_currentInput.empty())
        return;

    bool matched = checkMatch(m_currentInput);

    if (!matched)
    {
        onWrongMatch();
    }

    m_currentInput.clear();
}

bool GameEngine::isGameOver() const
{
    if (!m_isRunning)
        return true;

    // Endless mode: only check health
    if (m_stats.health <= 0.0f)
        return true;

    return false;
}

float GameEngine::getElapsedTime() const
{
    if (!m_isRunning)
        return 0.0f;

    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - m_startTime).count() - m_totalPausedTime;
    return elapsed;
}

GameRecord GameEngine::getResult() const
{
    GameRecord record;

    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - m_startTime).count() - m_totalPausedTime;
    float minutes = elapsed / 60.0f;

    record.wpm = (minutes > 0.0f) ? static_cast<int>(std::round(m_stats.correctWords / minutes)) : 0;
    record.accuracy = m_stats.getAccuracy();
    record.survivalTime = elapsed;
    record.date = getCurrentDateTime();
    record.correctWords = m_stats.correctWords;
    record.missedWords = m_stats.missedWords;
    record.wrongAttempts = m_stats.wrongAttempts;
    record.maxCombo = m_stats.maxCombo;

    return record;
}

bool GameEngine::shouldFlashRedBorder() const
{
    return m_flashRedBorder;
}

void GameEngine::spawnWord()
{
    if (m_wordManager.isEmpty())
        return;

    Word word = m_wordManager.getRandomWord();
    float y = getRandomYPosition();

    m_fallingWords.emplace_back(word, y, m_currentTeleportInterval);
}

void GameEngine::updateDifficulty(float deltaTime)
{
    // Teleport interval decreases over time (words teleport faster)
    m_currentTeleportInterval -= GameConfig::TELEPORT_INTERVAL_DECREASE * deltaTime;
    m_currentTeleportInterval = std::max(m_currentTeleportInterval, GameConfig::MIN_TELEPORT_INTERVAL);

    // Spawn interval decreases over time (more words spawn)
    m_currentSpawnIntervalMin -= GameConfig::SPAWN_INTERVAL_DECREASE * deltaTime;
    m_currentSpawnIntervalMax -= GameConfig::SPAWN_INTERVAL_DECREASE * deltaTime;
    m_currentSpawnIntervalMin = std::max(m_currentSpawnIntervalMin, GameConfig::MIN_SPAWN_INTERVAL);
    m_currentSpawnIntervalMax = std::max(m_currentSpawnIntervalMax, GameConfig::MIN_SPAWN_INTERVAL + 0.2f);
}

void GameEngine::updateFallingWords(float deltaTime)
{
    const int visibleWidth = std::max(1, m_visibleWidth.load());
    for (auto it = m_fallingWords.begin(); it != m_fallingWords.end();)
    {
        it->update(deltaTime, visibleWidth);

        // Check if word has left the screen
        if (!it->isActive)
        {
            onWordMissed();
            it = m_fallingWords.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

bool GameEngine::checkMatch(const std::string& input)
{
    for (size_t i = 0; i < m_fallingWords.size(); ++i)
    {
        auto& fw = m_fallingWords[i];

        // Only match visible words
        if (!fw.isVisible(m_visibleWidth.load()))
            continue;

        // Check for match
        if (fw.word.text == input)
        {
            removeWord(i);
            onCorrectMatch();
            return true;
        }
    }

    return false;
}

void GameEngine::removeWord(size_t index)
{
    if (index < m_fallingWords.size())
    {
        m_fallingWords.erase(m_fallingWords.begin() + index);
    }
}

void GameEngine::onCorrectMatch()
{
    m_stats.onCorrect();

    // Gain health
    m_stats.health = std::min(m_stats.health + GameConfig::HEALTH_GAIN, GameConfig::MAX_HEALTH_CAP);
}

void GameEngine::onWrongMatch()
{
    m_stats.onWrong();

    // Trigger red border flash
    m_flashRedBorder = true;
    m_flashStartTime = std::chrono::steady_clock::now();
}

void GameEngine::onWordMissed()
{
    m_stats.onMiss();

    // Lose health
    m_stats.health -= GameConfig::HEALTH_LOSS;
    if (m_stats.health < 0.0f)
    {
        m_stats.health = 0.0f;
    }
}

float GameEngine::getRandomSpawnInterval()
{
    std::uniform_real_distribution<float> dist(m_currentSpawnIntervalMin, m_currentSpawnIntervalMax);
    return dist(m_gen);
}

float GameEngine::getRandomYPosition()
{
    const int visibleHeight = std::max(1, m_visibleHeight.load());
    std::uniform_real_distribution<float> dist(0.0f, static_cast<float>(visibleHeight - 1));
    return dist(m_gen);
}

std::string GameEngine::getCurrentDateTime() const
{
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);

    std::tm timeInfo{};
#ifdef _WIN32
    localtime_s(&timeInfo, &time_t);
#else
    localtime_r(&time_t, &timeInfo);
#endif

    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%Y-%m-%d %H:%M");
    return oss.str();
}

void GameEngine::updateVisibleArea(int width, int height)
{
    width = std::max(10, width);
    height = std::max(1, height);
    m_visibleWidth.store(width);
    m_visibleHeight.store(height);
}
