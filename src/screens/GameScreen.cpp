#include "GameScreen.h"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/canvas.hpp"
#include "ftxui/dom/elements.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <utility>

using namespace ftxui;

GameScreen::GameScreen(
    GameEngine& engine,
    ftxui::ScreenInteractive& screen,
    std::function<void()> onGameFinished,
    std::function<void()> onAbort
)
    : m_engine(engine), m_screen(screen), m_onGameFinished(std::move(onGameFinished)), m_onAbort(std::move(onAbort))
{}

GameScreen::~GameScreen()
{
    onExit();
}

void GameScreen::onEnter()
{
    m_lastFrameTime = std::chrono::steady_clock::now();
    m_finishNotified = false;

    if (m_updateThreadRunning)
    {
        return;
    }

    m_updateThreadRunning = true;

    m_updateThread = std::thread(
        [this]()
        {
            auto lastUpdateTime = std::chrono::steady_clock::now();

            while (m_updateThreadRunning)
            {
                auto now = std::chrono::steady_clock::now();
                float deltaTime = std::chrono::duration<float>(now - lastUpdateTime).count();
                lastUpdateTime = now;

                m_screen.Post(
                    [this, deltaTime]()
                    {
                        if (m_engine.isRunning() && !m_engine.isGameOver())
                        {
                            m_engine.update(deltaTime);
                        }
                        else if (!m_finishNotified.exchange(true))
                        {
                            m_updateThreadRunning = false;
                            if (m_onGameFinished)
                            {
                                m_onGameFinished();
                            }
                        }
                    }
                );

                m_screen.Post(Event::Custom);
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
        }
    );
}

void GameScreen::onExit()
{
    m_updateThreadRunning = false;
    if (m_updateThread.joinable())
    {
        m_updateThread.join();
    }
}

Component GameScreen::createComponent()
{
    auto component = Container::Vertical({});

    auto renderer = Renderer(
        component,
        [this]
        {
            // Renderer only handles rendering, not game logic updates
            return render();
        }
    );

    renderer |= CatchEvent(
        [this](Event event)
        {
            if (m_engine.isGameOver())
            {
                return false;
            }

            if (event == Event::Character(' ') || event == Event::Return)
            {
                m_engine.handleSpace();
                return true;
            }

            if (event.is_character())
            {
                m_engine.handleCharInput(event.character()[0]);
                return true;
            }

            if (event == Event::Backspace)
            {
                m_engine.handleBackspace();
                return true;
            }

            if (event == Event::Escape)
            {
                m_engine.stop();
                m_updateThreadRunning = false;
                m_finishNotified = true;
                if (m_onAbort)
                {
                    m_onAbort();
                }
                return true;
            }

            return false;
        }
    );

    return renderer;
}

Element GameScreen::render()
{
    auto borderColor = m_engine.shouldFlashRedBorder() ? Color::Red : Color::White;

    return vbox({
               renderHeader(),
               separator(),
               renderGameArea() | flex,
               separator(),
               renderInputBox(),
               renderStats(),
           }) |
           border | color(borderColor);
}

Element GameScreen::renderHeader()
{
    float elapsed = m_engine.getElapsedTime();
    int minutes = static_cast<int>(elapsed) / 60;
    int seconds = static_cast<int>(elapsed) % 60;

    std::ostringstream timeStr;
    timeStr << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds;

    return hbox({
        text("Time: " + timeStr.str()) | bold | color(Color::Cyan),
        text("  "),
        renderHealthBar() | flex,
        text("  "),
        renderCombo(),
    });
}

Element GameScreen::renderHealthBar()
{
    float healthPercent = m_engine.getHealthPercentage();
    int totalBlocks = 20;
    int filledBlocks = static_cast<int>(healthPercent / 100.0f * totalBlocks);

    std::string bar;
    for (int i = 0; i < filledBlocks; ++i)
        bar += "█";
    for (int i = filledBlocks; i < totalBlocks; ++i)
        bar += "░";

    auto barElement = text(bar);

    if (healthPercent > 60)
    {
        barElement = barElement | color(Color::Green);
    }
    else if (healthPercent > 30)
    {
        barElement = barElement | color(Color::Yellow);
    }
    else
    {
        barElement = barElement | color(Color::Red);
    }

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(0) << healthPercent;

    return hbox({text("❤ "), barElement, text(" " + oss.str() + "%")});
}

Element GameScreen::renderCombo()
{
    int combo = m_engine.getStats().currentCombo;

    if (combo < GameConfig::MIN_COMBO_DISPLAY)
    {
        return text("");
    }

    auto comboText = text("Combo: " + std::to_string(combo) + "x");

    if (combo >= 50)
    {
        return comboText | color(Color::RedLight) | bold | blink;
    }
    else if (combo >= 30)
    {
        return comboText | color(Color::YellowLight) | bold;
    }
    else if (combo >= 10)
    {
        return comboText | color(Color::Yellow);
    }
    else
    {
        return comboText | color(Color::White);
    }
}

Element GameScreen::renderGameArea()
{
    // Use lambda overload, Canvas is created at render time
    // Get actual dimensions inside lambda and update engine's visible area
    return canvas(
               [this](Canvas& c)
               {
                   int w = c.width();
                   int h = c.height() / 2; // Canvas height is in pixels, convert to character height

                   // Update visible area at render time to handle resize correctly
                   m_engine.updateVisibleArea(w, h);

                   drawFallingWords(c, w, h);
               }
           ) |
           flex | reflect(m_gameAreaBox);
}

void GameScreen::drawFallingWords(ftxui::Canvas& canvas, int width, int height)
{
    const auto& words = m_engine.getFallingWords();
    for (const auto& fw : words)
    {
        if (!fw.isVisible(width))
        {
            continue;
        }

        const auto colorRGB = fw.getCurrentColor();
        const int x = std::clamp(static_cast<int>(std::round(fw.x)), 0, std::max(0, width - 1));
        const int y = std::clamp(static_cast<int>(std::round(fw.y * 2.0f)), 0, std::max(0, height * 2 - 2));

        canvas.DrawText(
            x,
            y,
            fw.word.text,
            Color::RGB(static_cast<uint8_t>(colorRGB.r), static_cast<uint8_t>(colorRGB.g), static_cast<uint8_t>(colorRGB.b))
        );
    }
}

Element GameScreen::renderInputBox()
{
    std::string displayInput = m_engine.getCurrentInput();
    if (displayInput.empty())
    {
        displayInput = " "; // Avoid empty line
    }

    return vbox({
        hbox({
            text("Input: ") | bold,
            text(displayInput) | color(Color::Cyan) | bold,
            text("_") | blink,
        }),
    });
}

Element GameScreen::renderStats()
{
    const auto& stats = m_engine.getStats();

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(1) << stats.getAccuracy();

    return hbox({
               text("Correct: " + std::to_string(stats.correctWords)),
               text("  "),
               text("Wrong: " + std::to_string(stats.wrongAttempts)),
               text("  "),
               text("Missed: " + std::to_string(stats.missedWords)),
               text("  "),
               text("Accuracy: " + oss.str() + "%"),
           }) |
           dim;
}
