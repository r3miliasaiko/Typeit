#pragma once

#include "BaseScreen.h"
#include "../managers/RecordManager.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_options.hpp"
#include <functional>

class StatsScreen : public BaseScreen
{
public:
    StatsScreen(RecordManager& recordManager, std::function<void()> onClose);

    ftxui::Component createComponent() override;
    void onEnter() override;

private:
    RecordManager& m_recordManager;
    std::function<void()> m_onClose;
    bool m_showTrendPoints = true;
    bool m_showMovingAverage = true;
    float m_tableScrollRatio = 0.0f;
    int m_tableTotalRows = 0;
    static constexpr int kTableVisibleRows = 12;
    static constexpr int kToggleBoxWidth = 28;
    static constexpr int kTrendCanvasMinWidth = 60;
    static constexpr int kTrendCanvasHeight = 12;

    ftxui::Element renderBestRecords();
    ftxui::Element renderTrendSection(const ftxui::Component& toggleComponent);
    ftxui::Element renderTrendCanvas();
    ftxui::Element renderRecordsTable();
};
