#pragma once

#include "Word.h"
#include <chrono>

namespace ftxui {
    class Color;
}

class FallingWord {
public:
    Word word;
    float x, y;
    float speed;
    float lifeProgress;  // 0.0 (刚生成) -> 1.0 (即将消失)
    bool isActive;
    std::chrono::steady_clock::time_point spawnTime;
    
    FallingWord() = default;
    FallingWord(const Word& word, float y, float speed);
    
    void update(float deltaTime, int screenWidth);
    bool isVisible(int screenWidth) const;
    
    // 返回RGB值（0-255）
    struct ColorRGB {
        int r, g, b;
    };
    ColorRGB getCurrentColor() const;
};

