#pragma once

#include <string>

class GameRecord
{
public:
    int wpm = 0;
    double accuracy = 0.0;
    float survivalTime = 0.0f; // Survival time in seconds
    std::string date;
    int correctWords = 0;
    int missedWords = 0;
    int wrongAttempts = 0;
    int maxCombo = 0;

    GameRecord() = default;

    std::string toCSVLine() const;
    static GameRecord fromCSVLine(const std::string& line);

    bool operator>(const GameRecord& other) const { return wpm > other.wpm; }
};
