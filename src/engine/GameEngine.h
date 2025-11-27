#pragma once

#include "../models/FallingWord.h"
#include "../models/GameStats.h"
#include "../models/GameRecord.h"
#include "../managers/WordManager.h"
#include "../utils/GameConfig.h"
#include <atomic>
#include <vector>
#include <string>
#include <chrono>
#include <random>

class GameEngine
{
public:
    GameEngine(WordManager& wordManager);

    // Game control (endless mode, no time limit)
    void start(int screenWidth);
    void update(float deltaTime);
    void pause();
    void resume();
    void stop();

    // Input handling
    void handleCharInput(char c);
    void handleBackspace();
    void handleSpace();

    // State queries
    bool isRunning() const { return m_isRunning; }
    bool isGameOver() const;
    float getElapsedTime() const;
    float getHealthPercentage() const { return m_stats.health; }
    float getCurrentTeleportInterval() const { return m_currentTeleportInterval; }
    int getVisibleWidth() const { return m_visibleWidth.load(); }
    int getVisibleHeight() const { return m_visibleHeight.load(); }

    // Data access
    const std::vector<FallingWord>& getFallingWords() const { return m_fallingWords; }
    const std::string& getCurrentInput() const { return m_currentInput; }
    const GameStats& getStats() const { return m_stats; }
    GameRecord getResult() const;
    bool shouldFlashRedBorder() const;
    void updateVisibleArea(int width, int height);

private:
    WordManager& m_wordManager;
    std::atomic<int> m_visibleWidth;
    std::atomic<int> m_visibleHeight;

    // Game state
    bool m_isRunning = false;
    bool m_isPaused = false;
    std::chrono::steady_clock::time_point m_startTime;
    std::chrono::steady_clock::time_point m_pauseTime;
    float m_totalPausedTime = 0.0f;

    // Difficulty scaling (initialized in start())
    float m_currentTeleportInterval = 1.5f;
    float m_currentSpawnIntervalMin = 2.0f;
    float m_currentSpawnIntervalMax = 4.0f;

    // Word management
    std::vector<FallingWord> m_fallingWords;
    float m_nextSpawnTime = 0.0f;
    std::random_device m_rd;
    std::mt19937 m_gen;

    // Input
    std::string m_currentInput;

    // Stats
    GameStats m_stats;

    // Effects
    bool m_flashRedBorder = false;
    std::chrono::steady_clock::time_point m_flashStartTime;

    // Private methods
    void spawnWord();
    void updateFallingWords(float deltaTime);
    void updateDifficulty(float deltaTime);
    bool checkMatch(const std::string& input);
    void removeWord(size_t index);
    void onCorrectMatch();
    void onWrongMatch();
    void onWordMissed();
    float getRandomSpawnInterval();
    float getRandomYPosition();
    std::string getCurrentDateTime() const;
};
