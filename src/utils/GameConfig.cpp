#include "GameConfig.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cctype>

ConfigManager& ConfigManager::instance()
{
    static ConfigManager inst;
    return inst;
}

std::string ConfigManager::trim(const std::string& str)
{
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

bool ConfigManager::parseBool(const std::string& value)
{
    std::string lower = value;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    return lower == "true" || lower == "1" || lower == "yes";
}

bool ConfigManager::loadFromFile(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        // Create default config if file doesn't exist
        createDefaultConfig(filepath);
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        line = trim(line);

        // Skip empty lines and comments
        if (line.empty() || line[0] == '#' || line[0] == ';' || line[0] == '[')
            continue;

        size_t eqPos = line.find('=');
        if (eqPos == std::string::npos)
            continue;

        std::string key = trim(line.substr(0, eqPos));
        std::string value = trim(line.substr(eqPos + 1));

        // Parse each setting
        try
        {
            // Teleport settings
            if (key == "base_teleport_interval")
                m_settings.baseTeleportInterval = std::stof(value);
            else if (key == "min_teleport_interval")
                m_settings.minTeleportInterval = std::stof(value);
            else if (key == "teleport_interval_decrease")
                m_settings.teleportIntervalDecrease = std::stof(value);
            else if (key == "teleport_step_ratio")
                m_settings.teleportStepRatio = std::stof(value);

            // Spawn settings
            else if (key == "spawn_interval_min")
                m_settings.spawnIntervalMin = std::stof(value);
            else if (key == "spawn_interval_max")
                m_settings.spawnIntervalMax = std::stof(value);
            else if (key == "spawn_interval_decrease")
                m_settings.spawnIntervalDecrease = std::stof(value);
            else if (key == "min_spawn_interval")
                m_settings.minSpawnInterval = std::stof(value);
            else if (key == "max_concurrent_words")
                m_settings.maxConcurrentWords = std::stoi(value);

            // Health settings
            else if (key == "max_health")
                m_settings.maxHealth = std::stof(value);
            else if (key == "health_gain")
                m_settings.healthGain = std::stof(value);
            else if (key == "health_loss")
                m_settings.healthLoss = std::stof(value);
            else if (key == "health_cap")
                m_settings.healthCap = std::stof(value);

            // Effects
            else if (key == "border_flash_duration")
                m_settings.borderFlashDuration = std::stof(value);
            else if (key == "min_combo_display")
                m_settings.minComboDisplay = std::stoi(value);

            // Color thresholds
            else if (key == "color_white_threshold")
                m_settings.colorWhiteThreshold = std::stof(value);
            else if (key == "color_yellow_threshold")
                m_settings.colorYellowThreshold = std::stof(value);

            // Game area
            else if (key == "game_area_height")
                m_settings.gameAreaHeight = std::stoi(value);
            else if (key == "header_height")
                m_settings.headerHeight = std::stoi(value);
            else if (key == "input_height")
                m_settings.inputHeight = std::stoi(value);
        }
        catch (...)
        {
            // Ignore parse errors, keep default value
        }
    }

    return true;
}

bool ConfigManager::saveToFile(const std::string& filepath) const
{
    // Ensure directory exists
    std::filesystem::path path(filepath);
    if (!path.parent_path().empty())
    {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream file(filepath);
    if (!file.is_open())
        return false;

    file << "# TypeIt Game Configuration\n";
    file << "# Edit these values to customize game difficulty\n\n";

    file << "[Teleport]\n";
    file << "# How words move across the screen (blink/teleport style)\n";
    file << "base_teleport_interval = " << m_settings.baseTeleportInterval << "\n";
    file << "min_teleport_interval = " << m_settings.minTeleportInterval << "\n";
    file << "teleport_interval_decrease = " << m_settings.teleportIntervalDecrease << "\n";
    file << "teleport_step_ratio = " << m_settings.teleportStepRatio << "\n\n";

    file << "[Spawn]\n";
    file << "# How often new words appear\n";
    file << "spawn_interval_min = " << m_settings.spawnIntervalMin << "\n";
    file << "spawn_interval_max = " << m_settings.spawnIntervalMax << "\n";
    file << "spawn_interval_decrease = " << m_settings.spawnIntervalDecrease << "\n";
    file << "min_spawn_interval = " << m_settings.minSpawnInterval << "\n";
    file << "max_concurrent_words = " << m_settings.maxConcurrentWords << "\n\n";

    file << "[Health]\n";
    file << "max_health = " << m_settings.maxHealth << "\n";
    file << "health_gain = " << m_settings.healthGain << "\n";
    file << "health_loss = " << m_settings.healthLoss << "\n";
    file << "health_cap = " << m_settings.healthCap << "\n\n";

    file << "[Effects]\n";
    file << "border_flash_duration = " << m_settings.borderFlashDuration << "\n";
    file << "min_combo_display = " << m_settings.minComboDisplay << "\n\n";

    file << "[Colors]\n";
    file << "# Thresholds for word color change (0.0 - 1.0)\n";
    file << "color_white_threshold = " << m_settings.colorWhiteThreshold << "\n";
    file << "color_yellow_threshold = " << m_settings.colorYellowThreshold << "\n\n";

    file << "[GameArea]\n";
    file << "game_area_height = " << m_settings.gameAreaHeight << "\n";
    file << "header_height = " << m_settings.headerHeight << "\n";
    file << "input_height = " << m_settings.inputHeight << "\n";

    return true;
}

void ConfigManager::createDefaultConfig(const std::string& filepath) const
{
    saveToFile(filepath);
}

