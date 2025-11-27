#include "Application.h"
#include <iostream>

using namespace ftxui;

Application::Application()
    : m_currentState(State::Menu)
    , m_isRunning(true)
    , m_screen(ScreenInteractive::Fullscreen())
    , m_gameEngine(m_wordManager)
    , m_isNewRecord(false)
{}

Application::~Application()
{
    if (m_activeScreen)
    {
        m_activeScreen->onExit();
    }
}

void Application::run()
{
    initialize();

    if (!loadResources())
    {
        std::cerr << "错误：无法加载资源文件！\n";
        std::cerr << "请确保 data/words.txt 文件存在。\n";
        return;
    }

    m_exitClosure = m_screen.ExitLoopClosure();
    m_rootComponent = buildRootComponent();
    showMenu();

    m_loopRunning = true;
    m_screen.Loop(m_rootComponent);
    m_loopRunning = false;
}

void Application::initialize()
{
    m_recordManager.loadRecords();
}

bool Application::loadResources()
{
    return m_wordManager.loadFromFile(GameConfig::WORDS_FILE);
}

ftxui::Component Application::buildRootComponent()
{
    auto renderer = Renderer(
        [this]
        {
            std::shared_lock lock(m_componentMutex);
            if (m_activeComponent)
            {
                return m_activeComponent->Render();
            }
            return ftxui::text("加载中...") | center;
        }
    );

    renderer |= CatchEvent(
        [this](Event event)
        {
            ftxui::Component component;
            {
                std::shared_lock lock(m_componentMutex);
                component = m_activeComponent;
            }
            if (component)
            {
                return component->OnEvent(event);
            }
            return false;
        }
    );

    return renderer;
}

void Application::setActiveComponent(ftxui::Component component)
{
    {
        std::unique_lock lock(m_componentMutex);
        m_activeComponent = component;
    }
    if (m_loopRunning)
    {
        m_screen.Post(Event::Custom);
    }
}

void Application::setScreen(std::shared_ptr<BaseScreen> screen)
{
    if (m_activeScreen)
    {
        m_activeScreen->onExit();
    }

    m_activeScreen = std::move(screen);

    if (m_activeScreen)
    {
        m_activeScreen->onEnter();
        setActiveComponent(m_activeScreen->createComponent());
    }
    else
    {
        setActiveComponent(nullptr);
    }
}

void Application::showMenu()
{
    auto menuScreen = std::make_shared<MenuScreen>(
        [this](MenuScreen::MenuOption option)
        {
            switch (option)
            {
            case MenuScreen::MenuOption::StartGame:
                startGame();
                break;
            case MenuScreen::MenuOption::ViewStats:
                showStatsScreen();
                break;
            case MenuScreen::MenuOption::Exit:
                m_isRunning = false;
                if (m_exitClosure)
                {
                    m_exitClosure();
                }
                break;
            }
        }
    );

    setScreen(menuScreen);
}

void Application::startGame()
{
    constexpr int kScreenWidth = 100;
    m_gameEngine.start(kScreenWidth);

    auto gameScreen = std::make_shared<GameScreen>(m_gameEngine, m_screen, [this]() { handleGameFinished(); }, [this]() { showMenu(); });

    m_gameScreen = gameScreen;
    setScreen(gameScreen);
}

void Application::handleGameFinished()
{
    m_lastGameRecord = m_gameEngine.getResult();
    m_isNewRecord = m_recordManager.isNewRecord(m_lastGameRecord);
    m_recordManager.saveRecord(m_lastGameRecord);

    showResultScreen(m_lastGameRecord, m_isNewRecord);
}

void Application::showResultScreen(const GameRecord& record, bool isNewRecord)
{
    auto resultScreen = std::make_shared<ResultScreen>(record, isNewRecord, [this]() { showMenu(); });

    setScreen(resultScreen);
}

void Application::showStatsScreen()
{
    auto statsScreen = std::make_shared<StatsScreen>(m_recordManager, [this]() { showMenu(); });

    setScreen(statsScreen);
}
