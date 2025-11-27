#pragma once

#include "../models/GameRecord.h"
#include "../utils/Constants.h"
#include <vector>
#include <string>
#include <utility>

class RecordManager {
public:
    RecordManager(const std::string& csvPath = GameConfig::RECORDS_FILE);
    
    bool loadRecords();
    bool saveRecord(const GameRecord& record);
    
    std::vector<GameRecord> getAllRecords() const;
    GameRecord getBestRecord() const;  // 最高 WPM
    GameRecord getLongestSurvivalRecord() const;  // 最长坚持时间
    bool isNewRecord(const GameRecord& record) const;
    
    // 统计方法
    std::vector<double> getRecentWPMAverage(int lastN = 100) const;
    std::vector<std::pair<std::string, double>> getWPMTimeSeries(int lastN = 100) const;
    
private:
    std::string m_csvPath;
    std::vector<GameRecord> m_records;
    
    void sortRecordsByDate();
    void ensureFileExists();
};

