#include "ResultScreen.h"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include <iomanip>
#include <sstream>
#include <utility>

using namespace ftxui;

ResultScreen::ResultScreen(const GameRecord& record, bool isNewRecord, std::function<void()> onContinue)
    : m_record(record), m_isNewRecord(isNewRecord), m_onContinue(std::move(onContinue))
{}

Component ResultScreen::createComponent()
{
    auto component = Container::Vertical({});

    auto renderer = Renderer(
        component,
        [this]
        {
            std::vector<Element> elements;

            elements.push_back(text(""));
            elements.push_back(renderTitle());
            elements.push_back(text(""));

            if (m_isNewRecord)
            {
                elements.push_back(renderNewRecordBadge());
                elements.push_back(text(""));
            }

            elements.push_back(renderStats());
            elements.push_back(text(""));
            elements.push_back(text("按 Enter 返回主菜单") | center | dim);
            elements.push_back(text(""));

            return vbox(std::move(elements)) | border | center;
        }
    );

    renderer |= CatchEvent(
        [this](Event event)
        {
            if (event == Event::Return || event == Event::Escape)
            {
                if (m_onContinue)
                {
                    m_onContinue();
                }
                return true;
            }
            return false;
        }
    );

    return renderer;
}

Element ResultScreen::renderTitle()
{
    return vbox({
        text("╔═══════════════════════════════════════╗") | center | bold,
        text("║                                       ║") | center,
        text("║          🎉 游戏结束 🎉              ║") | center | bold | color(Color::Yellow),
        text("║                                       ║") | center,
        text("╚═══════════════════════════════════════╝") | center | bold,
    });
}

Element ResultScreen::renderStats()
{
    std::ostringstream accOss;
    accOss << std::fixed << std::setprecision(1) << m_record.accuracy;

    std::string modeStr;
    switch (m_record.mode)
    {
    case GameConfig::GameMode::Thirty:
        modeStr = "30s";
        break;
    case GameConfig::GameMode::Sixty:
        modeStr = "60s";
        break;
    case GameConfig::GameMode::OneTwenty:
        modeStr = "120s";
        break;
    }

    return vbox({
        hbox({
            text("WPM: ") | bold,
            text(std::to_string(m_record.wpm)) | color(Color::Cyan) | bold,
        }) | center,
        text(""),
        hbox({
            text("准确率: ") | bold,
            text(accOss.str() + "%") | color(Color::Green) | bold,
        }) | center,
        text(""),
        hbox({
            text("模式: ") | bold,
            text(modeStr) | color(Color::Yellow),
        }) | center,
        text(""),
        hbox({
            text("最大连击: ") | bold,
            text(std::to_string(m_record.maxCombo) + "x") | color(Color::Red) | bold,
        }) | center,
        text(""),
        hbox({
            text("正确: ") | color(Color::Green),
            text(std::to_string(m_record.correctWords)),
            text("  错误: ") | color(Color::Red),
            text(std::to_string(m_record.wrongAttempts)),
            text("  错过: ") | color(Color::Yellow),
            text(std::to_string(m_record.missedWords)),
        }) | center |
            dim,
        text(""),
        text("日期: " + m_record.datetime) | center | dim,
    });
}

Element ResultScreen::renderNewRecordBadge()
{
    return vbox({
        text("⭐ ⭐ ⭐ 新纪录！ ⭐ ⭐ ⭐") | center | bold | color(Color::RedLight) | blink,
    });
}
