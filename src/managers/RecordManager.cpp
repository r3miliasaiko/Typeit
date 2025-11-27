#include "RecordManager.h"
#include <fstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>

RecordManager::RecordManager(const std::string& csvPath)
    : m_csvPath(csvPath) {
}

void RecordManager::ensureFileExists() {
    // 确保目录存在
    std::filesystem::path filePath(m_csvPath);
    if (!filePath.parent_path().empty()) {
        std::filesystem::create_directories(filePath.parent_path());
    }
    
    // 如果文件不存在，创建并写入头部
    std::ifstream testFile(m_csvPath);
    if (!testFile.good()) {
        std::ofstream file(m_csvPath);
        file << "WPM,Accuracy,SurvivalTime,Date,CorrectWords,MissedWords,WrongAttempts,MaxCombo\n";
    }
}

bool RecordManager::loadRecords() {
    std::ifstream file(m_csvPath);
    if (!file.is_open()) {
        return false;
    }
    
    m_records.clear();
    std::string line;
    
    // 跳过头部
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        try {
            GameRecord record = GameRecord::fromCSVLine(line);
            m_records.push_back(record);
        } catch (...) {
            // 忽略解析错误的行
        }
    }
    
    sortRecordsByDate();
    return true;
}

bool RecordManager::saveRecord(const GameRecord& record) {
    ensureFileExists();
    
    std::ofstream file(m_csvPath, std::ios::app);
    if (!file.is_open()) {
        return false;
    }
    
    file << record.toCSVLine() << "\n";
    m_records.push_back(record);
    
    return true;
}

std::vector<GameRecord> RecordManager::getAllRecords() const {
    return m_records;
}

GameRecord RecordManager::getBestRecord() const {
    GameRecord best;
    best.wpm = 0;
    
    for (const auto& record : m_records) {
        if (record.wpm > best.wpm) {
            best = record;
        }
    }
    
    return best;
}

GameRecord RecordManager::getLongestSurvivalRecord() const {
    GameRecord longest;
    longest.survivalTime = 0.0f;
    
    for (const auto& record : m_records) {
        if (record.survivalTime > longest.survivalTime) {
            longest = record;
        }
    }
    
    return longest;
}

bool RecordManager::isNewRecord(const GameRecord& record) const {
    GameRecord best = getBestRecord();
    return record.wpm > best.wpm;
}

std::vector<double> RecordManager::getRecentWPMAverage(int lastN) const {
    std::vector<double> result;
    int count = std::min(lastN, static_cast<int>(m_records.size()));
    
    if (count == 0) return result;
    
    // 从最后开始取N条记录
    for (int i = std::max(0, static_cast<int>(m_records.size()) - count); 
         i < static_cast<int>(m_records.size()); ++i) {
        result.push_back(m_records[i].wpm);
    }
    
    return result;
}

std::vector<std::pair<std::string, double>> RecordManager::getWPMTimeSeries(int lastN) const {
    std::vector<std::pair<std::string, double>> result;
    int count = std::min(lastN, static_cast<int>(m_records.size()));
    
    for (int i = std::max(0, static_cast<int>(m_records.size()) - count); 
         i < static_cast<int>(m_records.size()); ++i) {
        const auto& record = m_records[i];
        // 只取日期部分（前10个字符）
        std::string date = record.date.substr(0, std::min(10, static_cast<int>(record.date.length())));
        result.emplace_back(date, record.wpm);
    }
    
    return result;
}

void RecordManager::sortRecordsByDate() {
    std::sort(m_records.begin(), m_records.end(),
        [](const GameRecord& a, const GameRecord& b) {
            return a.date < b.date;
        });
}

