#include "StatsScreen.h"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <utility>

using namespace ftxui;

StatsScreen::StatsScreen(RecordManager& recordManager, std::function<void()> onClose)
    : m_recordManager(recordManager), m_onClose(std::move(onClose))
{}

void StatsScreen::onEnter() {}

Component StatsScreen::createComponent()
{
    auto component = Container::Vertical({});

    auto renderer = Renderer(
        component,
        [this]
        {
            return vbox({
                       text("") | center,
                       text("╔═══════════════════════════════════════════════════════════════╗") | center | bold,
                       text("║                      📊 统计数据                              ║") | center | bold | color(Color::Cyan),
                       text("╚═══════════════════════════════════════════════════════════════╝") | center | bold,
                       text(""),
                       renderBestRecords(),
                       text(""),
                       separator(),
                       text(""),
                       renderWPMChart(),
                       text(""),
                       separator(),
                       text(""),
                       renderRecordsList() | flex,
                       text(""),
                       text("按 Enter 或 ESC 返回主菜单") | center | dim,
                   }) |
                   border;
        }
    );

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
            return false;
        }
    );

    return renderer;
}

Element StatsScreen::renderBestRecords()
{
    auto record30 = m_recordManager.getBestRecord(GameConfig::GameMode::Thirty);
    auto record60 = m_recordManager.getBestRecord(GameConfig::GameMode::Sixty);
    auto record120 = m_recordManager.getBestRecord(GameConfig::GameMode::OneTwenty);

    auto formatRecord = [](const GameRecord& rec) -> std::string
    {
        if (rec.wpm == 0)
            return "暂无记录";

        std::ostringstream oss;
        oss << "WPM " << rec.wpm << "  准确率 " << std::fixed << std::setprecision(1) << rec.accuracy << "%  连击 " << rec.maxCombo << "x";
        return oss.str();
    };

    return vbox({
        text("最佳记录:") | bold | color(Color::Yellow),
        text(""),
        hbox({
            text("  30s 模式:  ") | bold,
            text(formatRecord(record30)) | (record30.wpm > 0 ? color(Color::Green) : dim),
        }),
        hbox({
            text("  60s 模式:  ") | bold,
            text(formatRecord(record60)) | (record60.wpm > 0 ? color(Color::Green) : dim),
        }),
        hbox({
            text(" 120s 模式:  ") | bold,
            text(formatRecord(record120)) | (record120.wpm > 0 ? color(Color::Green) : dim),
        }),
    });
}

Element StatsScreen::renderWPMChart()
{
    auto timeSeries = m_recordManager.getWPMTimeSeries(20); // 显示最近20条

    if (timeSeries.empty())
    {
        return vbox({
            text("近期 WPM 趋势:") | bold | color(Color::Yellow),
            text(""),
            text("暂无数据") | center | dim,
        });
    }

    return vbox({
        text("近期 WPM 趋势:") | bold | color(Color::Yellow),
        text(""),
        drawSimpleChart(timeSeries),
    });
}

Element StatsScreen::drawSimpleChart(const std::vector<std::pair<std::string, double>>& data)
{
    if (data.empty())
    {
        return text("暂无数据") | dim;
    }

    // 找到最大值和最小值
    double maxWPM = 0;
    double minWPM = 999999;
    for (const auto& [date, wpm] : data)
    {
        maxWPM = std::max(maxWPM, wpm);
        minWPM = std::min(minWPM, wpm);
    }

    if (maxWPM == minWPM)
    {
        minWPM = maxWPM - 10;
    }

    const int height = 8;
    std::vector<Element> lines;

    // 绘制图表（简单的字符图）
    for (int h = height - 1; h >= 0; --h)
    {
        std::string line;
        double threshold = minWPM + (maxWPM - minWPM) * h / (height - 1);

        // Y轴标签
        std::ostringstream oss;
        oss << std::setw(4) << static_cast<int>(threshold) << " │";
        line = oss.str();

        // 数据点
        for (size_t i = 0; i < data.size(); ++i)
        {
            double wpm = data[i].second;
            double normalizedWPM = (wpm - minWPM) / (maxWPM - minWPM) * (height - 1);

            if (std::abs(normalizedWPM - h) < 0.5)
            {
                line += " ●";
            }
            else
            {
                line += "  ";
            }
        }

        lines.push_back(text(line));
    }

    // X轴
    std::string xAxis = "     └";
    for (size_t i = 0; i < data.size(); ++i)
    {
        xAxis += "──";
    }
    lines.push_back(text(xAxis));

    // 日期标签（显示部分）
    std::string dateLabels = "      ";
    if (data.size() > 0)
    {
        dateLabels += data.front().first.substr(5, 5); // MM-DD
        if (data.size() > 1)
        {
            for (size_t i = 0; i < (data.size() - 2) * 2; ++i)
                dateLabels += " ";
            dateLabels += data.back().first.substr(5, 5);
        }
    }
    lines.push_back(text(dateLabels) | dim);

    return vbox(std::move(lines));
}

Element StatsScreen::renderRecordsList()
{
    auto records = m_recordManager.getAllRecords();

    if (records.empty())
    {
        return vbox({
            text("历史记录:") | bold | color(Color::Yellow),
            text(""),
            text("暂无记录") | center | dim,
        });
    }

    // 倒序显示（最新的在前）
    std::reverse(records.begin(), records.end());

    std::vector<Element> tableRows;

    // 表头
    tableRows.push_back(hbox({
        text(" WPM ") | bold | center,
        separator(),
        text(" 准确率 ") | bold | center,
        separator(),
        text(" 模式 ") | bold | center,
        separator(),
        text(" 连击 ") | bold | center,
        separator(),
        text("  日期时间         ") | bold | center,
    }));

    tableRows.push_back(separator());

    // 数据行（最多显示10条）
    int displayCount = std::min(10, static_cast<int>(records.size()));
    for (int i = 0; i < displayCount; ++i)
    {
        const auto& rec = records[i];

        std::ostringstream accOss;
        accOss << std::fixed << std::setprecision(1) << rec.accuracy << "%";

        std::string modeStr;
        switch (rec.mode)
        {
        case GameConfig::GameMode::Thirty:
            modeStr = " 30s";
            break;
        case GameConfig::GameMode::Sixty:
            modeStr = " 60s";
            break;
        case GameConfig::GameMode::OneTwenty:
            modeStr = "120s";
            break;
        }

        tableRows.push_back(hbox({
            text(" " + std::to_string(rec.wpm) + "  ") | center,
            separator(),
            text(" " + accOss.str() + "  ") | center,
            separator(),
            text(" " + modeStr + " ") | center,
            separator(),
            text("  " + std::to_string(rec.maxCombo) + "x  ") | center,
            separator(),
            text(" " + rec.datetime + " "),
        }));
    }

    if (records.size() > static_cast<size_t>(displayCount))
    {
        tableRows.push_back(separator());
        tableRows.push_back(
            text("... 还有 " + std::to_string(records.size() - static_cast<size_t>(displayCount)) + " 条记录") | center | dim
        );
    }

    return vbox({
        text("历史记录:") | bold | color(Color::Yellow),
        text(""),
        vbox(std::move(tableRows)) | border,
    });
}
