#include "MenuScreen.h"
#include "ftxui/component/component.hpp"
#include "ftxui/component/event.hpp"
#include "ftxui/dom/elements.hpp"
#include <utility>

using namespace ftxui;

MenuScreen::MenuScreen(std::function<void(MenuOption)> onSelect)
    : m_onSelect(std::move(onSelect)), m_entries({"Start Game", "Statistics", "Exit"})
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
                       text("+===========================================+") | center | bold,
                       text("|                                           |") | center,
                       text("|                   TYPEIT                  |") | center | bold | color(Color::Cyan),
                       text("|                                           |") | center,
                       text("+===========================================+") | center | bold,
                       text("") | center,
                       vbox({
                           text(m_selectedIndex == 0 ? "> Start Game" : "  Start Game") | center |
                               (m_selectedIndex == 0 ? color(Color::Yellow) | bold : color(Color::White)),
                           text("") | center,
                           text(m_selectedIndex == 1 ? "> Statistics" : "  Statistics") | center |
                               (m_selectedIndex == 1 ? color(Color::Yellow) | bold : color(Color::White)),
                           text(m_selectedIndex == 2 ? "> Exit" : "  Exit") | center |
                               (m_selectedIndex == 2 ? color(Color::Yellow) | bold : color(Color::White)),
                       }),
                       text("") | center,
                   }) |
                   border | center;
        }
    );

    renderer |= CatchEvent(
        [this](Event event)
        {
            if (event == Event::Return)
            {
                MenuOption option = MenuOption::StartGame;
                switch (m_selectedIndex)
                {
                case 0:
                    option = MenuOption::StartGame;
                    break;
                case 1:
                    option = MenuOption::ViewStats;
                    break;
                case 2:
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
