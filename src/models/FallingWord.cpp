#include "FallingWord.h"
#include "../utils/Constants.h"
#include <algorithm>
#include <cmath>

FallingWord::FallingWord(const Word& word, float y, float speed)
    : word(word), x(0.0f), y(y), speed(speed), 
      lifeProgress(0.0f), isActive(true),
      spawnTime(std::chrono::steady_clock::now()) {
}

void FallingWord::update(float deltaTime, int screenWidth) {
    if (!isActive) return;
    
    // 更新位置
    x += speed * deltaTime;
    
    // 更新生命进度
    float maxDistance = static_cast<float>(screenWidth);
    lifeProgress = std::min(x / maxDistance, 1.0f);
    
    // 检查是否离开屏幕
    if (x > maxDistance) {
        isActive = false;
    }
}

bool FallingWord::isVisible(int screenWidth) const {
    return isActive && x >= 0 && x < screenWidth;
}

FallingWord::ColorRGB FallingWord::getCurrentColor() const {
    using namespace GameConfig;
    
    if (lifeProgress < COLOR_WHITE_THRESHOLD) {
        // 白色阶段
        return {255, 255, 255};
    } else if (lifeProgress < COLOR_YELLOW_THRESHOLD) {
        // 白色 -> 黄色
        float t = (lifeProgress - COLOR_WHITE_THRESHOLD) / (COLOR_YELLOW_THRESHOLD - COLOR_WHITE_THRESHOLD);
        int r = 255;
        int g = 255;
        int b = static_cast<int>(255 * (1.0f - t));
        return {r, g, b};
    } else {
        // 黄色 -> 红色
        float t = (lifeProgress - COLOR_YELLOW_THRESHOLD) / (1.0f - COLOR_YELLOW_THRESHOLD);
        int r = 255;
        int g = static_cast<int>(255 * (1.0f - t));
        int b = 0;
        return {r, g, b};
    }
}

