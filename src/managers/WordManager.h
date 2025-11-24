#pragma once

#include "../models/Word.h"
#include <vector>
#include <random>
#include <string>

class WordManager {
public:
    WordManager();
    
    bool loadFromFile(const std::string& filepath);
    Word getRandomWord();
    size_t getWordCount() const { return m_words.size(); }
    bool isEmpty() const { return m_words.empty(); }
    
private:
    std::vector<Word> m_words;
    std::random_device m_rd;
    std::mt19937 m_gen;
    
    void updateDistribution();
};

