#pragma once

#include <string>

class Word {
public:
    std::string text;
    std::string definition;
    int length;
    
    Word() = default;
    Word(const std::string& text, const std::string& def);
    
    int getDifficulty() const;
};

