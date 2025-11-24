#include "GameEngine.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>

GameEngine::GameEngine(WordManager& wordManager)
    : m_wordManager(wordManager), m_gen(m_rd()) {
}

void GameEngine::start(GameConfig::GameMode mode, int screenWidth) {
    m_mode = mode;
    m_screenWidth = screenWidth;
    m_isRunning = true;
    m_isPaused = false;
    m_startTime = std::chrono::steady_clock::now();
    m_totalPausedTime = 0.0f;
    
    // 重置状态
    m_fallingWords.clear();
    m_currentInput.clear();
    m_stats = GameStats();
    m_stats.health = GameConfig::MAX_HEALTH;
    
    m_nextSpawnTime = getRandomSpawnInterval();
    m_flashRedBorder = false;
}

void GameEngine::update(float deltaTime) {
    if (!m_isRunning || m_isPaused) return;
    
    // 更新飘动的单词
    updateFallingWords(deltaTime);
    
    // 检查是否需要生成新单词
    m_nextSpawnTime -= deltaTime;
    if (m_nextSpawnTime <= 0.0f && m_fallingWords.size() < GameConfig::MAX_CONCURRENT_WORDS) {
        spawnWord();
        m_nextSpawnTime = getRandomSpawnInterval();
    }
    
    // 更新边框闪红效果
    if (m_flashRedBorder) {
        auto now = std::chrono::steady_clock::now();
        float elapsed = std::chrono::duration<float>(now - m_flashStartTime).count();
        if (elapsed >= GameConfig::BORDER_FLASH_DURATION) {
            m_flashRedBorder = false;
        }
    }
}

void GameEngine::pause() {
    if (m_isRunning && !m_isPaused) {
        m_isPaused = true;
        m_pauseTime = std::chrono::steady_clock::now();
    }
}

void GameEngine::resume() {
    if (m_isRunning && m_isPaused) {
        auto now = std::chrono::steady_clock::now();
        m_totalPausedTime += std::chrono::duration<float>(now - m_pauseTime).count();
        m_isPaused = false;
    }
}

void GameEngine::stop() {
    m_isRunning = false;
}

void GameEngine::handleCharInput(char c) {
    if (!m_isRunning || m_isPaused) return;
    
    // 只接受字母
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
        m_currentInput += static_cast<char>(std::tolower(c));
    }
}

void GameEngine::handleBackspace() {
    if (!m_isRunning || m_isPaused) return;
    
    if (!m_currentInput.empty()) {
        m_currentInput.pop_back();
    }
}

void GameEngine::handleSpace() {
    if (!m_isRunning || m_isPaused) return;
    if (m_currentInput.empty()) return;
    
    bool matched = checkMatch(m_currentInput);
    
    if (!matched) {
        onWrongMatch();
    }
    
    m_currentInput.clear();
}

bool GameEngine::isGameOver() const {
    if (!m_isRunning) return true;
    
    // 检查血量
    if (m_stats.health <= 0.0f) return true;
    
    // 检查时间
    if (getTimeRemaining() <= 0.0f) return true;
    
    return false;
}

float GameEngine::getTimeRemaining() const {
    if (!m_isRunning) return 0.0f;
    
    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - m_startTime).count() - m_totalPausedTime;
    float duration = static_cast<float>(GameConfig::getDuration(m_mode));
    
    return std::max(0.0f, duration - elapsed);
}

GameRecord GameEngine::getResult() const {
    GameRecord record;
    
    auto now = std::chrono::steady_clock::now();
    float elapsed = std::chrono::duration<float>(now - m_startTime).count() - m_totalPausedTime;
    float minutes = elapsed / 60.0f;
    
    record.wpm = (minutes > 0.0f) ? static_cast<int>(std::round(m_stats.correctWords / minutes)) : 0;
    record.accuracy = m_stats.getAccuracy();
    record.mode = m_mode;
    record.datetime = getCurrentDateTime();
    record.correctWords = m_stats.correctWords;
    record.missedWords = m_stats.missedWords;
    record.wrongAttempts = m_stats.wrongAttempts;
    record.maxCombo = m_stats.maxCombo;
    
    return record;
}

bool GameEngine::shouldFlashRedBorder() const {
    return m_flashRedBorder;
}

void GameEngine::spawnWord() {
    if (m_wordManager.isEmpty()) return;
    
    Word word = m_wordManager.getRandomWord();
    float y = getRandomYPosition();
    float speed = GameConfig::BASE_WORD_SPEED;
    
    m_fallingWords.emplace_back(word, y, speed);
}

void GameEngine::updateFallingWords(float deltaTime) {
    for (auto it = m_fallingWords.begin(); it != m_fallingWords.end(); ) {
        it->update(deltaTime, m_screenWidth);
        
        // 检查是否离开屏幕
        if (!it->isActive) {
            onWordMissed();
            it = m_fallingWords.erase(it);
        } else {
            ++it;
        }
    }
}

bool GameEngine::checkMatch(const std::string& input) {
    for (size_t i = 0; i < m_fallingWords.size(); ++i) {
        auto& fw = m_fallingWords[i];
        
        // 只匹配可见的单词
        if (!fw.isVisible(m_screenWidth)) continue;
        
        // 检查匹配
        if (fw.word.text == input) {
            removeWord(i);
            onCorrectMatch();
            return true;
        }
    }
    
    return false;
}

void GameEngine::removeWord(size_t index) {
    if (index < m_fallingWords.size()) {
        m_fallingWords.erase(m_fallingWords.begin() + index);
    }
}

void GameEngine::onCorrectMatch() {
    m_stats.onCorrect();
    
    // 加血
    m_stats.health = std::min(m_stats.health + GameConfig::HEALTH_GAIN, 
                               GameConfig::MAX_HEALTH_CAP);
}

void GameEngine::onWrongMatch() {
    m_stats.onWrong();
    
    // 触发边框闪红
    m_flashRedBorder = true;
    m_flashStartTime = std::chrono::steady_clock::now();
}

void GameEngine::onWordMissed() {
    m_stats.onMiss();
    
    // 扣血
    m_stats.health -= GameConfig::HEALTH_LOSS;
    if (m_stats.health < 0.0f) {
        m_stats.health = 0.0f;
    }
}

float GameEngine::getRandomSpawnInterval() {
    std::uniform_real_distribution<float> dist(
        GameConfig::SPAWN_INTERVAL_MIN, 
        GameConfig::SPAWN_INTERVAL_MAX
    );
    return dist(m_gen);
}

float GameEngine::getRandomYPosition() {
    std::uniform_real_distribution<float> dist(0.0f, 
        static_cast<float>(GameConfig::GAME_AREA_HEIGHT - 1));
    return dist(m_gen);
}

std::string GameEngine::getCurrentDateTime() const {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::tm timeInfo{};
#ifdef _WIN32
    localtime_s(&timeInfo, &time_t);
#else
    localtime_r(&time_t, &timeInfo);
#endif
    
    std::ostringstream oss;
    oss << std::put_time(&timeInfo, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
}

