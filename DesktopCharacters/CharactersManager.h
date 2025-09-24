#pragma once
#include "Window/Windows_Window.h"
using WindowClass = Windows_Window;

#include "PlatformInterface/Windows_PlatformInterface.h"
using PlatformInterfaceClass = Windows_PlatformInterface;

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

    // Drag
    Character* draggedCharacter = nullptr;
    Vec2 dragOffset; // Offset from mouse to character position when drag started
    struct DragSample
    {
        Vec2 position;
        float time; // in seconds
    };
    std::vector<DragSample> dragHistory;
    const float dragHistoryDuration = 0.1f; // track last 0.1 seconds for velocity

    //
public:
    CharactersManager();
    ~CharactersManager();

    bool initialize();

    bool addCharacter(const Vec2& position, const Vec2& velocity);
    void closeAllCharacters();
    void updateCharacters(float deltaTime);
    void renderCharacters();
    size_t getCharacterCount() const;

    int runLoop();
private:
    bool checkExitKeys();

    void onWindowEvent(const WindowEvent& evt);

    void interactLeftMouse(const Vec2& mousePos);

    void updateDragging(float deltaTime);

    Vec2 screenToWorld(const Vec2& screen) const;
    Vec2 worldToScreen(const Vec2& world) const;
};