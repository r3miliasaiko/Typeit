#pragma once

#include "ftxui/component/component.hpp"

class BaseScreen
{
public:
    virtual ~BaseScreen() = default;
    virtual ftxui::Component createComponent() = 0;
    virtual void onEnter() {}
    virtual void onExit() {}
};
