#include "FallingWord.h"
#include "../utils/GameConfig.h"
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
    
    const auto& cfg = ConfigManager::instance().settings();
    
    auto now = std::chrono::steady_clock::now();
    float timeSinceLastTeleport = std::chrono::duration<float>(now - lastTeleportTime).count();
    
    // Check if it's time to teleport
    if (timeSinceLastTeleport >= teleportInterval) {
        // Teleport to next position
        float stepSize = static_cast<float>(screenWidth) * cfg.teleportStepRatio;
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
    const auto& cfg = ConfigManager::instance().settings();
    
    if (lifeProgress < cfg.colorWhiteThreshold) {
        // White phase
        return {255, 255, 255};
    } else if (lifeProgress < cfg.colorYellowThreshold) {
        // White -> Yellow
        float t = (lifeProgress - cfg.colorWhiteThreshold) / (cfg.colorYellowThreshold - cfg.colorWhiteThreshold);
        int r = 255;
        int g = 255;
        int b = static_cast<int>(255 * (1.0f - t));
        return {r, g, b};
    } else {
        // Yellow -> Red
        float t = (lifeProgress - cfg.colorYellowThreshold) / (1.0f - cfg.colorYellowThreshold);
        int r = 255;
        int g = static_cast<int>(255 * (1.0f - t));
        int b = 0;
        return {r, g, b};
    }
}

