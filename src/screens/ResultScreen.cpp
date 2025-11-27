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
            elements.push_back(text("Press Enter to return to menu") | center | dim);
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
        text("+=======================================+") | center | bold,
        text("|                                       |") | center,
        text("|            GAME OVER                  |") | center | bold | color(Color::Yellow),
        text("|                                       |") | center,
        text("+=======================================+") | center | bold,
    });
}

Element ResultScreen::renderStats()
{
    std::ostringstream accOss;
    accOss << std::fixed << std::setprecision(1) << m_record.accuracy;

    // Format survival time
    int totalSeconds = static_cast<int>(m_record.survivalTime);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    std::ostringstream timeOss;
    timeOss << minutes << "m " << seconds << "s";

    return vbox({
        hbox({
            text("WPM: ") | bold,
            text(std::to_string(m_record.wpm)) | color(Color::Cyan) | bold,
        }) | center,
        text(""),
        hbox({
            text("Survival Time: ") | bold,
            text(timeOss.str()) | color(Color::Yellow) | bold,
        }) | center,
        text(""),
        hbox({
            text("Accuracy: ") | bold,
            text(accOss.str() + "%") | color(Color::Green) | bold,
        }) | center,
        text(""),
        hbox({
            text("Max Combo: ") | bold,
            text(std::to_string(m_record.maxCombo) + "x") | color(Color::Red) | bold,
        }) | center,
        text(""),
        hbox({
            text("Correct: ") | color(Color::Green),
            text(std::to_string(m_record.correctWords)),
            text("  Wrong: ") | color(Color::Red),
            text(std::to_string(m_record.wrongAttempts)),
            text("  Missed: ") | color(Color::Yellow),
            text(std::to_string(m_record.missedWords)),
        }) | center |
            dim,
        text(""),
        text("Date: " + m_record.date) | center | dim,
    });
}

Element ResultScreen::renderNewRecordBadge()
{
    return vbox({
        text("*** NEW RECORD! ***") | center | bold | color(Color::RedLight) | blink,
    });
}
