cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


project(Typeit LANGUAGES CXX)

add_executable(${PROJECT_NAME})


target_sources(${PROJECT_NAME} PRIVATE 
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

option(FTXUI_ENABLE_INSTALL OFF)
include(FetchContent)
FetchContent_Declare(ftxui
  GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
  GIT_TAG v6.1.9
)
FetchContent_MakeAvailable(ftxui)



target_link_libraries(${PROJECT_NAME} PRIVATE
    ftxui::component
    ftxui::dom
    ftxui::screen
)




target_compile_options(${PROJECT_NAME} PUBLIC 
    $<$<COMPILE_LANG_AND_ID:CXX,MSVC>: /W4 /WX /MP>)

# Install rules
include(GNUInstallDirs)

# Install executable
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION .
)

# Install data files to same directory as executable
install(DIRECTORY ${CMAKE_SOURCE_DIR}/data
    DESTINATION .
)

# Copy data files to build directory for development
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/data
    $<TARGET_FILE_DIR:${PROJECT_NAME}>/data
    COMMENT "Copying data files to build directory"
)

# CPack configuration
set(CPACK_PACKAGE_NAME "Typeit")
set(CPACK_PACKAGE_VENDOR "Typeit")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Terminal-based typing game")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "0")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_GENERATOR "ZIP")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)

include(CPack)

