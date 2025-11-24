#include "WordManager.h"
#include <fstream>
#include <sstream>

WordManager::WordManager() : m_gen(m_rd()) {
}

bool WordManager::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    m_words.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        // 格式：word definition
        std::istringstream iss(line);
        std::string word, definition;
        
        if (iss >> word) {
            // 读取剩余部分作为释义
            std::getline(iss, definition);
            // 去除前导空格
            if (!definition.empty() && definition[0] == ' ') {
                definition = definition.substr(1);
            }
            
            m_words.emplace_back(word, definition);
        }
    }
    
    updateDistribution();
    return !m_words.empty();
}

Word WordManager::getRandomWord() {
    if (m_words.empty()) {
        return Word("ERROR", "No words loaded");
    }
    
    std::uniform_int_distribution<size_t> dist(0, m_words.size() - 1);
    return m_words[dist(m_gen)];
}

void WordManager::updateDistribution() {
    // Distribution is created in getRandomWord()
}

