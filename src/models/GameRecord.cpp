#include "GameRecord.h"
#include <sstream>
#include <iomanip>
#include <vector>

std::string GameRecord::toCSVLine() const {
    std::ostringstream oss;
    oss << wpm << ","
        << std::fixed << std::setprecision(1) << accuracy << ","
        << static_cast<int>(mode) << ","
        << datetime << ","
        << correctWords << ","
        << missedWords << ","
        << wrongAttempts << ","
        << maxCombo;
    return oss.str();
}

GameRecord GameRecord::fromCSVLine(const std::string& line) {
    GameRecord record;
    std::istringstream iss(line);
    std::string token;
    std::vector<std::string> tokens;
    
    while (std::getline(iss, token, ',')) {
        tokens.push_back(token);
    }
    
    if (tokens.size() >= 8) {
        record.wpm = std::stoi(tokens[0]);
        record.accuracy = std::stod(tokens[1]);
        
        int modeValue = std::stoi(tokens[2]);
        if (modeValue == 30) record.mode = GameConfig::GameMode::Thirty;
        else if (modeValue == 60) record.mode = GameConfig::GameMode::Sixty;
        else record.mode = GameConfig::GameMode::OneTwenty;
        
        record.datetime = tokens[3];
        record.correctWords = std::stoi(tokens[4]);
        record.missedWords = std::stoi(tokens[5]);
        record.wrongAttempts = std::stoi(tokens[6]);
        record.maxCombo = std::stoi(tokens[7]);
    }
    
    return record;
}

