cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)





set(ProjectName "Typeit")


project(${ProjectName} LANGUAGES CXX)

add_executable(${ProjectName})


target_sources(${ProjectName} PRIVATE 
    src/main.cpp
    src/Application.cpp
    src/models/Word.cpp
    src/models/FallingWord.cpp
    src/models/GameRecord.cpp
    src/managers/WordManager.cpp
    src/managers/RecordManager.cpp
    src/engine/GameEngine.cpp
    src/screens/MenuScreen.cpp
    src/screens/GameScreen.cpp
    src/screens/ResultScreen.cpp
    src/screens/StatsScreen.cpp
    src/utils/GameConfig.cpp
)


include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)


target_link_libraries(${ProjectName} PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
)




target_compile_options(${ProjectName} PUBLIC 
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>: /W4 /WX /MP>)

