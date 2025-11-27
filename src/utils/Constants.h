#pragma once

#include <cstddef>

namespace GameConfig
{
// Word teleport configuration (blink-style movement)
constexpr float BASE_TELEPORT_INTERVAL = 1.5f;       // Initial teleport interval (seconds)
constexpr float MIN_TELEPORT_INTERVAL = 0.3f;        // Minimum teleport interval
constexpr float TELEPORT_INTERVAL_DECREASE = 0.005f; // Interval decrease per second
constexpr float TELEPORT_STEP_RATIO = 0.15f;         // Each teleport moves 15% of screen width
constexpr float SPAWN_INTERVAL_MIN = 2.0f;           // Min spawn interval (seconds)
constexpr float SPAWN_INTERVAL_MAX = 4.0f;           // Max spawn interval (seconds)
constexpr float SPAWN_INTERVAL_DECREASE = 0.01f;     // Spawn interval decrease per second
constexpr float MIN_SPAWN_INTERVAL = 0.8f;           // Minimum spawn interval
constexpr int MAX_CONCURRENT_WORDS = 8;              // Max concurrent words on screen

// Health system
constexpr float MAX_HEALTH = 100.0f;
constexpr float HEALTH_GAIN = 8.0f;      // Health gained on correct input
constexpr float HEALTH_LOSS = 15.0f;     // Health lost on miss
constexpr float MAX_HEALTH_CAP = 100.0f; // Health cap

// Effects
constexpr float BORDER_FLASH_DURATION = 0.3f; // Red border flash duration (seconds)
constexpr int MIN_COMBO_DISPLAY = 5;          // Minimum combo to display

// Color gradient thresholds
constexpr float COLOR_WHITE_THRESHOLD = 0.33f;  // 0-33% white
constexpr float COLOR_YELLOW_THRESHOLD = 0.66f; // 33-66% white->yellow
                                                // 66-100% yellow->red

// File paths
constexpr const char* WORDS_FILE = "data/words.txt";
constexpr const char* RECORDS_FILE = "data/records.csv";

// Game area dimensions
constexpr int GAME_AREA_HEIGHT = 15; // Word area height
constexpr int HEADER_HEIGHT = 3;     // Header height
constexpr int INPUT_HEIGHT = 4;      // Input area height
} // namespace GameConfig
