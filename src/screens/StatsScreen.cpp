#include "StatsScreen.h"

#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"

#include "ftxui/dom/canvas.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/dom/table.hpp"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <sstream>
#include <utility>

using namespace ftxui;

StatsScreen::StatsScreen(RecordManager& recordManager, std::function<void()> onClose)
    : m_recordManager(recordManager), m_onClose(std::move(onClose))
{}

void StatsScreen::onEnter() {}

Component StatsScreen::createComponent()
{
    auto toggleComponent = Container::Horizontal({
        Checkbox("Show Data Points", &m_showTrendPoints),
        Checkbox("Show Moving Avg (50)", &m_showMovingAverage),
    });

    auto tableRenderer = Renderer([this] { return renderRecordsTable(); });

    SliderOption<float> sliderOption;
    sliderOption.value = &m_tableScrollRatio;
    sliderOption.min = 0.0f;
    sliderOption.max = 1.0f;
    sliderOption.increment = 0.05f;
    sliderOption.direction = Direction::Down;
    sliderOption.color_active = Color::Yellow;
    sliderOption.color_inactive = Color::GrayDark;
    sliderOption.on_change = [this]() { m_tableScrollRatio = std::clamp(m_tableScrollRatio, 0.0f, 1.0f); };
    auto sliderComponent = Slider(sliderOption);

    // Main scroll container for the entire content
    auto contentRenderer = Renderer(
        [this, toggleComponent, tableRenderer, sliderComponent]
        {
            auto tableSection = hbox({
                tableRenderer->Render() | flex,
                sliderComponent->Render() | size(WIDTH, EQUAL, 3),
            });

            return vbox({
                text("") | center,
                text("+===============================================================+") | center | bold,
                text("|                       STATISTICS                              |") | center | bold | color(Color::Cyan),
                text("+===============================================================+") | center | bold,
                text(""),
                renderBestRecords(),
                text(""),
                separator(),
                renderTrendSection(toggleComponent),
                text(""),
                tableSection | flex,
                text(""),
                text("Controls: Enter/Esc=Back | P=Toggle Points | A=Toggle Avg | PgUp/PgDn/J/K=Scroll") | center | dim,
            });
        }
    );

    auto composite = Container::Vertical({
        toggleComponent,
        Container::Horizontal({
            tableRenderer,
            sliderComponent,
        }),
    });

    // Wrap in yframe for vertical scrolling when terminal is too small
    auto renderer = Renderer(composite, [contentRenderer] { return contentRenderer->Render() | yframe | border; });

    renderer |= CatchEvent(
        [this](Event event)
        {
            if (event == Event::Return || event == Event::Escape)
            {
                if (m_onClose)
                {
                    m_onClose();
                }
                return true;
            }

            if (event == Event::Character('p') || event == Event::Character('P'))
            {
                m_showTrendPoints = !m_showTrendPoints;
                return true;
            }
            if (event == Event::Character('a') || event == Event::Character('A'))
            {
                m_showMovingAverage = !m_showMovingAverage;
                return true;
            }

            auto applyScrollDelta = [this](float delta)
            {
                if (m_tableTotalRows <= kTableVisibleRows)
                {
                    m_tableScrollRatio = 0.0f;
                    return false;
                }
                m_tableScrollRatio = std::clamp(m_tableScrollRatio + delta, 0.0f, 1.0f);
                return true;
            };

            if (event == Event::PageDown || event == Event::Character('j') || event == Event::ArrowDown)
            {
                applyScrollDelta(0.15f);
                return true;
            }
            if (event == Event::PageUp || event == Event::Character('k') || event == Event::ArrowUp)
            {
                applyScrollDelta(-0.15f);
                return true;
            }

            return false;
        }
    );

    return renderer;
}

Element StatsScreen::renderBestRecords()
{
    auto bestRecord = m_recordManager.getBestRecord();
    auto longestRecord = m_recordManager.getLongestSurvivalRecord();

    auto formatWPMRecord = [](const GameRecord& rec) -> std::string
    {
        if (rec.wpm == 0)
            return "No records yet";

        std::ostringstream oss;
        oss << "WPM " << rec.wpm << "  Accuracy " << std::fixed << std::setprecision(1) << rec.accuracy << "%  Combo " << rec.maxCombo
            << "x";
        return oss.str();
    };

    auto formatTimeRecord = [](const GameRecord& rec) -> std::string
    {
        if (rec.survivalTime <= 0.0f)
            return "No records yet";

        int totalSeconds = static_cast<int>(rec.survivalTime);
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;

        std::ostringstream oss;
        oss << minutes << "m " << seconds << "s  WPM " << rec.wpm << "  Combo " << rec.maxCombo << "x";
        return oss.str();
    };

    return vbox({
        text("Best Records:") | bold | color(Color::Yellow),
        text(""),
        hbox({
            text("  Highest WPM:      ") | bold,
            text(formatWPMRecord(bestRecord)) | (bestRecord.wpm > 0 ? color(Color::Green) : dim),
        }),
        hbox({
            text("  Longest Survival: ") | bold,
            text(formatTimeRecord(longestRecord)) | (longestRecord.survivalTime > 0.0f ? color(Color::Cyan) : dim),
        }),
    });
}

Element StatsScreen::renderTrendSection(const Component& toggleComponent)
{
    auto controls = hbox({
                        toggleComponent->Render() | center,
                    }) |
                    size(WIDTH, LESS_THAN, kToggleBoxWidth);

    return vbox({
        text("Recent WPM Trend:") | bold | color(Color::Yellow),
        text(""),
        vbox({

            renderTrendCanvas() | flex, controls
        }),
    });
}

Element StatsScreen::renderTrendCanvas()
{
    auto records = m_recordManager.getAllRecords();
    if (records.empty())
    {
        return text("No data yet") | center | dim | flex;
    }

    const int pointCount = static_cast<int>(records.size());
    const int width = std::max(kTrendCanvasMinWidth, pointCount);
    const int height = kTrendCanvasHeight;

    double minWPM = std::numeric_limits<double>::max();
    double maxWPM = std::numeric_limits<double>::lowest();
    for (const auto& record : records)
    {
        minWPM = std::min(minWPM, static_cast<double>(record.wpm));
        maxWPM = std::max(maxWPM, static_cast<double>(record.wpm));
    }
    if (maxWPM - minWPM < 5.0)
    {
        maxWPM += 5.0;
        minWPM = std::max(0.0, maxWPM - 20.0);
    }

    // Calculate moving average
    std::vector<double> movingAverage(pointCount, 0.0);
    const int window = 50;
    double sum = 0.0;
    for (int i = 0; i < pointCount; ++i)
    {
        sum += records[i].wpm;
        if (i >= window)
        {
            sum -= records[i - window].wpm;
        }
        int divisor = std::min(window, i + 1);
        movingAverage[i] = sum / divisor;
    }

    // Extract WPM data for lambda capture (avoid capturing entire records)
    std::vector<int> wpmData;
    wpmData.reserve(pointCount);
    for (const auto& r : records)
    {
        wpmData.push_back(r.wpm);
    }

    bool showPoints = m_showTrendPoints;
    bool showMA = m_showMovingAverage;

    return canvas(
               [=](Canvas& c)
               {
                   auto normalizeY = [&](double value)
                   {
                       double ratio = (value - minWPM) / (maxWPM - minWPM);
                       ratio = std::clamp(ratio, 0.0, 1.0);
                       return static_cast<int>((height * 2 - 3) - ratio * (height * 2 - 4));
                   };

                   auto normalizeX = [&](int index)
                   {
                       if (pointCount <= 1)
                           return 0;
                       double ratio = static_cast<double>(index) / (pointCount - 1);
                       return static_cast<int>(ratio * (width - 1));
                   };

                   // Axes
                   c.DrawPointLine(0, height * 2 - 2, width - 1, height * 2 - 2, Color::GrayDark);
                   c.DrawPointLine(0, 0, 0, height * 2 - 2, Color::GrayDark);
                   c.DrawText(1, 0, std::to_string(static_cast<int>(std::round(maxWPM))), Color::GrayLight);
                   c.DrawText(1, height * 2 - 4, std::to_string(static_cast<int>(std::round(minWPM))), Color::GrayLight);

                   if (showPoints)
                   {
                       for (int i = 0; i < pointCount; ++i)
                       {
                           int x = normalizeX(i);
                           int y = normalizeY(wpmData[i]);
                           c.DrawPoint(x, y, true, Color::YellowLight);
                       }
                   }

                   if (showMA && pointCount > 1)
                   {
                       for (int i = 1; i < pointCount; ++i)
                       {
                           int x1 = normalizeX(i - 1);
                           int y1 = normalizeY(movingAverage[i - 1]);
                           int x2 = normalizeX(i);
                           int y2 = normalizeY(movingAverage[i]);
                           c.DrawPointLine(x1, y1, x2, y2, Color::Cyan);
                       }
                   }
               }
           ) |
           size(WIDTH, EQUAL, width) | size(HEIGHT, EQUAL, height) | flex;
}
Element StatsScreen::renderRecordsTable()
{
    auto records = m_recordManager.getAllRecords();
    if (records.empty())
        return text("No records yet") | center | dim;

    m_tableTotalRows = static_cast<int>(records.size());
    const int maxStart = std::max(0, m_tableTotalRows - kTableVisibleRows);
    if (maxStart == 0)
        m_tableScrollRatio = 0.0f;
    else
        m_tableScrollRatio = std::clamp(m_tableScrollRatio, 0.0f, 1.0f);

    const int startIndex = (maxStart == 0) ? 0 : static_cast<int>(std::round(m_tableScrollRatio * maxStart));
    const int endIndex = std::min(startIndex + kTableVisibleRows, m_tableTotalRows);

    auto formatTime = [](float seconds) -> std::string
    {
        int totalSeconds = static_cast<int>(seconds);
        int mins = totalSeconds / 60;
        int secs = totalSeconds % 60;
        std::ostringstream oss;
        oss << mins << ":" << std::setfill('0') << std::setw(2) << secs;
        return oss.str();
    };

    std::vector<std::vector<std::string>> rows;
    rows.push_back({"WPM", "Accuracy", "Survival", "Combo", "Date"});
    for (int i = startIndex; i < endIndex; ++i)
    {
        const auto& rec = records[i];
        std::ostringstream acc;
        acc << std::fixed << std::setprecision(1) << rec.accuracy << "%";
        rows.push_back({
            std::to_string(rec.wpm),
            acc.str(),
            formatTime(rec.survivalTime),
            std::to_string(rec.maxCombo) + "x",
            rec.date,
        });
    }

    Table table(rows);
    table.SelectAll().DecorateCells(center);
    table.SelectAll().DecorateCells(size(HEIGHT, GREATER_THAN, 1));

    table.SelectAll().DecorateCells(size(WIDTH, GREATER_THAN, 6));
    table.SelectRow(0).Decorate(bold | color(Color::YellowLight));

    const Element summary =
        text("Showing " + std::to_string(startIndex + 1) + "-" + std::to_string(endIndex) + " of " + std::to_string(m_tableTotalRows)) |
        dim;

    return vbox({
        hbox({
            filler(),
            table.Render() | flex | size(WIDTH, GREATER_THAN, 0),
            filler(),
        }) | flex,
        summary | center,
    });
}