#pragma once

#include "../models/FallingWord.h"
#include "../models/GameStats.h"
#include "../models/GameRecord.h"
#include "../managers/WordManager.h"
#include "../utils/Constants.h"
#include <vector>
#include <string>
#include <chrono>
#include <random>

class GameEngine {
public:
    GameEngine(WordManager& wordManager);
    
    // 游戏控制
    void start(GameConfig::GameMode mode, int screenWidth);
    void update(float deltaTime);
    void pause();
    void resume();
    void stop();
    
    // 输入处理
    void handleCharInput(char c);
    void handleBackspace();
    void handleSpace();
    
    // 状态查询
    bool isRunning() const { return m_isRunning; }
    bool isGameOver() const;
    float getTimeRemaining() const;
    float getHealthPercentage() const { return m_stats.health; }
    int getScreenWidth() const { return m_screenWidth; }
    
    // 数据访问
    const std::vector<FallingWord>& getFallingWords() const { return m_fallingWords; }
    const std::string& getCurrentInput() const { return m_currentInput; }
    const GameStats& getStats() const { return m_stats; }
    GameRecord getResult() const;
    bool shouldFlashRedBorder() const;
    
private:
    // 配置
    GameConfig::GameMode m_mode;
    WordManager& m_wordManager;
    int m_screenWidth;
    
    // 游戏状态
    bool m_isRunning = false;
    bool m_isPaused = false;
    std::chrono::steady_clock::time_point m_startTime;
    std::chrono::steady_clock::time_point m_pauseTime;
    float m_totalPausedTime = 0.0f;
    
    // 单词管理
    std::vector<FallingWord> m_fallingWords;
    float m_nextSpawnTime = 0.0f;
    std::random_device m_rd;
    std::mt19937 m_gen;
    
    // 输入
    std::string m_currentInput;
    
    // 统计
    GameStats m_stats;
    
    // 特效
    bool m_flashRedBorder = false;
    std::chrono::steady_clock::time_point m_flashStartTime;
    
    // 私有方法
    void spawnWord();
    void updateFallingWords(float deltaTime);
    bool checkMatch(const std::string& input);
    void removeWord(size_t index);
    void onCorrectMatch();
    void onWrongMatch();
    void onWordMissed();
    float getRandomSpawnInterval();
    float getRandomYPosition();
    std::string getCurrentDateTime() const;
};

