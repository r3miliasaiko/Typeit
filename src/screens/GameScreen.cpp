#include "GameScreen.h"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
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
            // Renderer 只负责渲染，不负责游戏逻辑更新
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
    int timeLeft = static_cast<int>(m_engine.getTimeRemaining());

    return hbox({
        text("时间: " + std::to_string(timeLeft) + "s") | bold,
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

    auto comboText = text("🔥 连击: " + std::to_string(combo) + "x");

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
    return renderFallingWords() | size(HEIGHT, EQUAL, GameConfig::GAME_AREA_HEIGHT);
}

Element GameScreen::renderFallingWords()
{
    std::vector<Element> lines;

    // 初始化所有行为空
    for (int i = 0; i < GameConfig::GAME_AREA_HEIGHT; ++i)
    {
        lines.push_back(text(""));
    }

    // 渲染每个飘动的单词
    for (const auto& fw : m_engine.getFallingWords())
    {
        if (!fw.isVisible(m_engine.getScreenWidth()))
            continue;

        int yPos = static_cast<int>(fw.y);
        int xPos = static_cast<int>(fw.x);

        if (yPos >= 0 && yPos < GameConfig::GAME_AREA_HEIGHT)
        {
            auto colorRGB = fw.getCurrentColor();

            // 创建单词元素并设置位置和颜色
            std::string spaces(xPos, ' ');
            auto wordElement =
                text(spaces + fw.word.text) |
                color(Color::RGB(static_cast<uint8_t>(colorRGB.r), static_cast<uint8_t>(colorRGB.g), static_cast<uint8_t>(colorRGB.b)));

            lines[yPos] = wordElement;
        }
    }

    return vbox(std::move(lines));
}

Element GameScreen::renderInputBox()
{
    std::string displayInput = m_engine.getCurrentInput();
    if (displayInput.empty())
    {
        displayInput = " "; // 避免空行
    }

    return vbox({
        hbox({
            text("输入: ") | bold,
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
               text("正确: " + std::to_string(stats.correctWords)),
               text("  "),
               text("错误: " + std::to_string(stats.wrongAttempts)),
               text("  "),
               text("错过: " + std::to_string(stats.missedWords)),
               text("  "),
               text("准确率: " + oss.str() + "%"),
           }) |
           dim;
}
