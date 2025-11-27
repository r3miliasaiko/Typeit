#pragma once

#include "../models/GameRecord.h"
#include "../utils/GameConfig.h"
#include <vector>
#include <string>
#include <utility>

class RecordManager {
public:
    RecordManager(const std::string& csvPath = GamePaths::RECORDS_FILE);
    
    bool loadRecords();
    bool saveRecord(const GameRecord& record);
    
    std::vector<GameRecord> getAllRecords() const;
    GameRecord getBestRecord() const;
    GameRecord getLongestSurvivalRecord() const;
    bool isNewRecord(const GameRecord& record) const;
    
    // Stats methods
    std::vector<double> getRecentWPMAverage(int lastN = 100) const;
    std::vector<std::pair<std::string, double>> getWPMTimeSeries(int lastN = 100) const;
    
private:
    std::string m_csvPath;
    std::vector<GameRecord> m_records;
    
    void sortRecordsByDate();
    void ensureFileExists();
};

