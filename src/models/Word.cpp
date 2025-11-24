#include "Word.h"

Word::Word(const std::string& text, const std::string& def)
    : text(text), definition(def), length(static_cast<int>(text.length())) {
}

int Word::getDifficulty() const {
    // 基于长度返回难度等级 1-5
    if (length <= 4) return 1;
    if (length <= 6) return 2;
    if (length <= 8) return 3;
    if (length <= 10) return 4;
    return 5;
}

