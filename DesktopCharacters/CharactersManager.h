#pragma once
#include "Window/WindowsWindow.h"
using WindowClass = WindowsWindow;

#include "PlatformInterface/WindowsPlatformInterface.h"
using PlatformInterfaceClass = WindowsPlatformInterface;

#include "Character.h"

#include <vector>
#include <memory>

class CharactersManager
{
    std::unique_ptr<BasePlatformInterface> platformInterface;
    std::unique_ptr<BaseWindow> mainWindow;

    Vec2 screenSize;

    std::vector<WindowData> windowsData;

    std::vector<std::unique_ptr<Character>> characters;
    bool shouldExit;
public:
    CharactersManager();
    ~CharactersManager();

    bool initialize();

    bool addCharacter();
    void closeAllCharacters();
    void updateCharacters(float deltaTime);
    size_t getCharacterCount() const;

    int runLoop();
private:
    bool checkExitKeys();

    Vec2 screenToWorld(const Vec2& screen) const;
    Vec2 worldToScreen(const Vec2& world) const;
};