#pragma once
#include "Window/Windows_Window.h"
using WindowClass = Windows_Window;

#include "PlatformInterface/Windows_PlatformInterface.h"
using PlatformInterfaceClass = Windows_PlatformInterface;

#include "Character.h"

#include <vector>
#include <memory>

struct InGameWindowData
{
    WindowData data;
    Vec2 velocity;
    Vec2 lastPosition;
};

class CharactersManager
{
public:
    CharactersManager();
    ~CharactersManager();

    bool initialize();

    bool addCharacter(const Vec2& position, const Vec2& velocity, const Character::Data& charData);

    int runLoop();
private:
    // Core platform and window management
    std::unique_ptr<BasePlatformInterface> platformInterface;
    std::unique_ptr<BaseWindow> mainWindow;
    Vec2 screenSize;

    // State
    bool shouldExit;

    // Windows' data
    std::vector<WindowData> windowsData;
    std::vector<InGameWindowData> inGameWindowsData;

    // Characters
    std::vector<std::unique_ptr<Character>> characters;
    

    // Dragging
    Character* draggedCharacter = nullptr;
    Vec2 dragOffset; // Offset from mouse to character position when drag started
    struct DragSample
    {
        Vec2 position;
        float time; // in seconds
    };
    std::vector<DragSample> dragHistory;
    const float dragHistoryDuration = 0.1f; // track last 0.1 seconds for velocity
private:
    void collectWindowsData(float deltaTime);
    void removeContainedWindows();

    void update(float deltaTime);
    void updateObstacles();
    void updateDragging(float deltaTime);

    void render();

    bool checkExitKeys();

    void onWindowEvent(const WindowEvent& evt);

    void interactLeftMouse(const Vec2& mousePos);

    float map(float value, float min1, float max1, float min2, float max2) const;
    Vec2 map(const Vec2& value, const Vec2& min1, const Vec2& max1, const Vec2& min2, const Vec2& max2) const;
    Vec2 screenToWorld(const Vec2& screen) const;
    Vec2 worldToScreen(const Vec2& world) const;
};