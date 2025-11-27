#pragma once

#include "engine/GameEngine.h"
#include "managers/RecordManager.h"
#include "managers/WordManager.h"
#include "screens/GameScreen.h"
#include "screens/MenuScreen.h"
#include "screens/ResultScreen.h"
#include "screens/StatsScreen.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>

class Application {
public:
    Application();
    ~Application();
    
    void run();
    
private:
    enum class State {
        Menu,
        Game,
        Result,
        Stats
    };
    
    State m_currentState;
    bool m_isRunning;
    ftxui::ScreenInteractive m_screen;
    
    // Managers
    WordManager m_wordManager;
    RecordManager m_recordManager;
    GameEngine m_gameEngine;
    
    // Result data
    GameRecord m_lastGameRecord;
    bool m_isNewRecord;
    
    ftxui::Component m_rootComponent;
    ftxui::Component m_activeComponent;
    std::shared_mutex m_componentMutex;
    std::function<void()> m_exitClosure;
    bool m_loopRunning = false;
    std::shared_ptr<GameScreen> m_gameScreen;
    std::shared_ptr<BaseScreen> m_activeScreen;

    void initialize();
    bool loadResources();
    ftxui::Component buildRootComponent();
    void setActiveComponent(ftxui::Component component);
    void setScreen(std::shared_ptr<BaseScreen> screen);

    void showMenu();
    void startGame();
    void showResultScreen(const GameRecord& record, bool isNewRecord);
    void showStatsScreen();
    void handleGameFinished();
};

