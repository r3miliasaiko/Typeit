#include "MenuScreen.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include <utility>

using namespace ftxui;

MenuScreen::MenuScreen(std::function<void(MenuOption)> onSelect)
    : m_onSelect(std::move(onSelect)), m_entries({"30s 模式", "60s 模式", "120s 模式", "统计数据", "退出游戏"})
{}

Component MenuScreen::createComponent()
{
    auto menu = Menu(&m_entries, &m_selectedIndex);

    auto component = Container::Vertical({
        menu,
    });

    auto renderer = Renderer(
        component,
        [this]
        {
            return vbox({
                       text("") | center,
                       text("╔═══════════════════════════════════════╗") | center | bold,
                       text("║                                       ║") | center,
                       text("║         🎮  TYPE MASTER  🎮          ║") | center | bold | color(Color::Cyan),
                       text("║                                       ║") | center,
                       text("╚═══════════════════════════════════════╝") | center | bold,
                       text("") | center,
                       vbox({
                           text(m_selectedIndex == 0 ? "➤ 30s 模式" : "  30s 模式") | center |
                               (m_selectedIndex == 0 ? color(Color::Yellow) | bold : color(Color::White)),
                           text(m_selectedIndex == 1 ? "➤ 60s 模式" : "  60s 模式") | center |
                               (m_selectedIndex == 1 ? color(Color::Yellow) | bold : color(Color::White)),
                           text(m_selectedIndex == 2 ? "➤ 120s 模式" : "  120s 模式") | center |
                               (m_selectedIndex == 2 ? color(Color::Yellow) | bold : color(Color::White)),
                           text("") | center,
                           text(m_selectedIndex == 3 ? "➤ 统计数据" : "  统计数据") | center |
                               (m_selectedIndex == 3 ? color(Color::Yellow) | bold : color(Color::White)),
                           text(m_selectedIndex == 4 ? "➤ 退出游戏" : "  退出游戏") | center |
                               (m_selectedIndex == 4 ? color(Color::Yellow) | bold : color(Color::White)),
                       }),
                       text("") | center,
                       text("使用 ↑↓ 选择，Enter 确认") | center | dim,
                   }) |
                   border | center;
        }
    );

    renderer |= CatchEvent(
        [this](Event event)
        {
            if (event == Event::Return)
            {
                MenuOption option = MenuOption::Start30s;
                switch (m_selectedIndex)
                {
                case 0:
                    option = MenuOption::Start30s;
                    break;
                case 1:
                    option = MenuOption::Start60s;
                    break;
                case 2:
                    option = MenuOption::Start120s;
                    break;
                case 3:
                    option = MenuOption::ViewStats;
                    break;
                case 4:
                    option = MenuOption::Exit;
                    break;
                }
                if (m_onSelect)
                {
                    m_onSelect(option);
                }
                return true;
            }
            return false;
        }
    );

    return renderer;
}

Element MenuScreen::renderContent()
{
    return vbox({
        text("TYPE MASTER") | center | bold,
    });
}
