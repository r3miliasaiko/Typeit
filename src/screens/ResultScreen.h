#pragma once

#include "BaseScreen.h"
#include "../models/GameRecord.h"
#include "ftxui/component/component.hpp"
#include <functional>

class ResultScreen : public BaseScreen {
public:
    ResultScreen(const GameRecord& record, bool isNewRecord, std::function<void()> onContinue);
    
    ftxui::Component createComponent() override;
    
private:
    GameRecord m_record;
    bool m_isNewRecord;
    std::function<void()> m_onContinue;
    
    ftxui::Element renderTitle();
    ftxui::Element renderStats();
    ftxui::Element renderNewRecordBadge();
};

