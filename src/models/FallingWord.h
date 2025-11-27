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
    float lifeProgress;  // 0.0 (just spawned) -> 1.0 (about to disappear)
    bool isActive;
    std::chrono::steady_clock::time_point spawnTime;
    std::chrono::steady_clock::time_point lastTeleportTime;
    float teleportInterval;  // Current teleport interval for this word
    int teleportCount;       // Number of teleports done
    
    FallingWord() = default;
    FallingWord(const Word& word, float y, float teleportInterval);
    
    void update(float deltaTime, int screenWidth);
    bool isVisible(int screenWidth) const;
    
    // Returns RGB values (0-255)
    struct ColorRGB {
        int r, g, b;
    };
    ColorRGB getCurrentColor() const;
};

