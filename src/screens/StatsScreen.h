#pragma once

#include "BaseScreen.h"
#include "../managers/RecordManager.h"
#include "ftxui/component/component.hpp"
#include <functional>

class StatsScreen : public BaseScreen {
public:
    StatsScreen(RecordManager& recordManager, std::function<void()> onClose);
    
    ftxui::Component createComponent() override;
    void onEnter() override;
    
private:
    RecordManager& m_recordManager;
    std::function<void()> m_onClose;
    
    ftxui::Element renderBestRecords();
    ftxui::Element renderWPMChart();
    ftxui::Element renderRecordsList();
    ftxui::Element drawSimpleChart(const std::vector<std::pair<std::string, double>>& data);
};

