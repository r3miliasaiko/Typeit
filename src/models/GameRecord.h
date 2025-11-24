#pragma once

#include "../utils/Constants.h"
#include <string>

class GameRecord {
public:
    int wpm;
    double accuracy;
    GameConfig::GameMode mode;
    std::string datetime;
    int correctWords;
    int missedWords;
    int wrongAttempts;
    int maxCombo;
    
    GameRecord() = default;
    
    std::string toCSVLine() const;
    static GameRecord fromCSVLine(const std::string& line);
    
    bool operator>(const GameRecord& other) const {
        return wpm > other.wpm;
    }
};

