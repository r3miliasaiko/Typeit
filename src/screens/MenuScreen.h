#pragma once

#include "BaseScreen.h"
#include "../utils/Constants.h"
#include "ftxui/component/component.hpp"
#include <functional>
#include <vector>

class MenuScreen : public BaseScreen {
public:
    enum class MenuOption {
        Start30s,
        Start60s,
        Start120s,
        ViewStats,
        Exit
    };
    
    explicit MenuScreen(std::function<void(MenuOption)> onSelect);
    
    ftxui::Component createComponent() override;
    
private:
    int m_selectedIndex = 0;
    std::function<void(MenuOption)> m_onSelect;
    std::vector<std::string> m_entries;
    
    ftxui::Element renderContent();
};

