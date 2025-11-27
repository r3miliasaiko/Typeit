#include "FallingWord.h"
#include "../utils/Constants.h"
#include <algorithm>
#include <cmath>

FallingWord::FallingWord(const Word& word, float y, float teleportInterval)
    : word(word), x(0.0f), y(y),
      lifeProgress(0.0f), isActive(true),
      spawnTime(std::chrono::steady_clock::now()),
      lastTeleportTime(std::chrono::steady_clock::now()),
      teleportInterval(teleportInterval),
      teleportCount(0) {
}

void FallingWord::update(float deltaTime, int screenWidth) {
    (void)deltaTime; // Not used for teleport-style movement
    if (!isActive) return;
    
    auto now = std::chrono::steady_clock::now();
    float timeSinceLastTeleport = std::chrono::duration<float>(now - lastTeleportTime).count();
    
    // Check if it's time to teleport
    if (timeSinceLastTeleport >= teleportInterval) {
        // Teleport to next position
        float stepSize = static_cast<float>(screenWidth) * GameConfig::TELEPORT_STEP_RATIO;
        x += stepSize;
        lastTeleportTime = now;
        teleportCount++;
    }
    
    // Update life progress based on position
    float maxDistance = static_cast<float>(screenWidth);
    lifeProgress = std::min(x / maxDistance, 1.0f);
    
    // Check if word has left the screen
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
        // White phase
        return {255, 255, 255};
    } else if (lifeProgress < COLOR_YELLOW_THRESHOLD) {
        // White -> Yellow
        float t = (lifeProgress - COLOR_WHITE_THRESHOLD) / (COLOR_YELLOW_THRESHOLD - COLOR_WHITE_THRESHOLD);
        int r = 255;
        int g = 255;
        int b = static_cast<int>(255 * (1.0f - t));
        return {r, g, b};
    } else {
        // Yellow -> Red
        float t = (lifeProgress - COLOR_YELLOW_THRESHOLD) / (1.0f - COLOR_YELLOW_THRESHOLD);
        int r = 255;
        int g = static_cast<int>(255 * (1.0f - t));
        int b = 0;
        return {r, g, b};
    }
}

