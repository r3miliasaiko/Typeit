#pragma once

#include <string>

// Runtime-configurable game settings loaded from config file
struct GameSettings
{
    // Word teleport configuration (blink-style movement)
    float baseTeleportInterval = 1.5f;       // Initial teleport interval (seconds)
    float minTeleportInterval = 0.3f;        // Minimum teleport interval
    float teleportIntervalDecrease = 0.005f; // Interval decrease per second
    float teleportStepRatio = 0.15f;         // Each teleport moves X% of screen width

    // Word spawn configuration
    float spawnIntervalMin = 2.0f;       // Min spawn interval (seconds)
    float spawnIntervalMax = 4.0f;       // Max spawn interval (seconds)
    float spawnIntervalDecrease = 0.01f; // Spawn interval decrease per second
    float minSpawnInterval = 0.8f;       // Minimum spawn interval floor
    int maxConcurrentWords = 8;          // Max concurrent words on screen

    // Health system
    float maxHealth = 100.0f;
    float healthGain = 8.0f;  // Health gained on correct input
    float healthLoss = 15.0f; // Health lost on miss
    float healthCap = 100.0f; // Health cap

    // Effects
    float borderFlashDuration = 0.3f; // Red border flash duration (seconds)
    int minComboDisplay = 5;          // Minimum combo to display

    // Color gradient thresholds (0.0 - 1.0)
    float colorWhiteThreshold = 0.33f;  // 0-33% white
    float colorYellowThreshold = 0.66f; // 33-66% white->yellow, 66-100% yellow->red

    // Game area dimensions
    int gameAreaHeight = 15;
    int headerHeight = 3;
    int inputHeight = 4;
};

// Compile-time constants that don't need to be configurable
namespace GamePaths
{
constexpr const char* WORDS_FILE = "data/words.txt";
constexpr const char* RECORDS_FILE = "data/records.csv";
constexpr const char* CONFIG_FILE = "data/config.ini";
} // namespace GamePaths

class ConfigManager
{
public:
    static ConfigManager& instance();

    bool loadFromFile(const std::string& filepath = GamePaths::CONFIG_FILE);
    bool saveToFile(const std::string& filepath = GamePaths::CONFIG_FILE) const;
    void createDefaultConfig(const std::string& filepath = GamePaths::CONFIG_FILE) const;

    const GameSettings& settings() const { return m_settings; }
    GameSettings& settings() { return m_settings; }

private:
    ConfigManager() = default;
    GameSettings m_settings;

    static std::string trim(const std::string& str);
    static bool parseBool(const std::string& value);
};

