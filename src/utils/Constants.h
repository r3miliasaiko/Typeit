#pragma once

#include <cstddef>

namespace GameConfig
{
// 游戏模式
enum class GameMode
{
    Thirty = 30,
    Sixty = 60,
    OneTwenty = 120
};

constexpr int getDuration(GameMode mode)
{
    return static_cast<int>(mode);
}

// 单词飘动配置
constexpr float BASE_WORD_SPEED = 15.0f;   // 像素/秒
constexpr float SPAWN_INTERVAL_MIN = 1.5f; // 最小生成间隔（秒）
constexpr float SPAWN_INTERVAL_MAX = 3.0f; // 最大生成间隔（秒）
constexpr int MAX_CONCURRENT_WORDS = 8;    // 最多同时显示单词数

// 生命系统
constexpr float MAX_HEALTH = 100.0f;
constexpr float HEALTH_GAIN = 8.0f;      // 正确加血
constexpr float HEALTH_LOSS = 15.0f;     // 错过扣血
constexpr float MAX_HEALTH_CAP = 100.0f; // 血量上限

// 特效
constexpr float BORDER_FLASH_DURATION = 0.3f; // 边框闪红持续时间（秒）
constexpr int MIN_COMBO_DISPLAY = 5;          // 最小显示连击数

// 颜色渐变阈值
constexpr float COLOR_WHITE_THRESHOLD = 0.33f;  // 0-33% 白色
constexpr float COLOR_YELLOW_THRESHOLD = 0.66f; // 33-66% 白->黄
                                                // 66-100% 黄->红

// 文件路径
constexpr const char* WORDS_FILE = "data/words.txt";
constexpr const char* RECORDS_FILE = "data/records.csv";

// 游戏区域大小
constexpr int GAME_AREA_HEIGHT = 15; // 单词飘动区域高度
constexpr int HEADER_HEIGHT = 3;     // 顶部信息栏高度
constexpr int INPUT_HEIGHT = 4;      // 底部输入区高度
} // namespace GameConfig
