#pragma once

#include "BaseScreen.h"
#include "../engine/GameEngine.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/screen/box.hpp"
#include <atomic>
#include <chrono>
#include <functional>
#include <thread>

class GameScreen : public BaseScreen
{
public:
    GameScreen(GameEngine& engine, ftxui::ScreenInteractive& screen, std::function<void()> onGameFinished, std::function<void()> onAbort);
    ~GameScreen() override;

    ftxui::Component createComponent() override;
    void onEnter() override;
    void onExit() override;

private:
    GameEngine& m_engine;
    ftxui::ScreenInteractive& m_screen;
    std::function<void()> m_onGameFinished;
    std::function<void()> m_onAbort;

    std::chrono::steady_clock::time_point m_lastFrameTime;
    std::atomic<bool> m_updateThreadRunning = false;
    std::atomic<bool> m_finishNotified = false;
    std::thread m_updateThread;
    ftxui::Box m_gameAreaBox;

    ftxui::Element render();
    ftxui::Element renderHeader();
    ftxui::Element renderHealthBar();
    ftxui::Element renderCombo();
    ftxui::Element renderGameArea();
    void drawFallingWords(ftxui::Canvas& canvas, int width, int height);
    ftxui::Element renderInputBox();
    ftxui::Element renderStats();
};
