#pragma once
#include "Window/BaseWindow.h"

#include "Core/Vec2.h"

#include <memory>
#include <vector>

#include "PlatformInterface//WindowsPlatformInterface.h"
using PlatformInterface = WindowsPlatformInterface;

// Character represents a desktop character entity
class Character
{
public:
    static PlatformInterface* platform;
    static std::vector<WindowData> windowsData;

    static Vec2 worldSize; // Center is at zero
    static Vec2 screenSize;

    Character();
    ~Character();

    // Create the character with a window
    bool create(const Vec2& position, const Vec2& size);

    void onWindowEvent(const WindowEvent& evt);

    // Character properties
    void setPosition(float x, float y);
    void setPosition(const Vec2& newPosition);

    void setSize(float width, float height);
    void setSize(const Vec2& newSize);

    void setVelocity(float vx, float vy);
    void setVelocity(const Vec2& newVelocity);

    void move(float deltaX, float deltaY);
    void move(const Vec2& delta);

    // Get character properties
    const Vec2& getPosition();
    const Vec2& getSize();
    const Vec2& getVelocity();

    // Update physics and window position
    void update(float deltaTime);

    // Character state
    bool isAlive() const;
    void destroy();

    // Window access (for manager)
    BaseWindow* getWindow() const;

    // Drag functionality
    void startDrag(const Vec2& mousePos);
    bool isDragging() const;
private:
    std::unique_ptr<BaseWindow> window;

    // Character position and size
    Vec2 position;
    Vec2 size;
    Vec2 velocity;

    // Character state
    bool alive;

    // Drag state
    bool dragging;
    Vec2 dragOffset; // Offset from mouse to character position when drag started
    struct DragSample
    {
        Vec2 position;
        float time; // in seconds
    };
    std::vector<DragSample> dragHistory;
    float dragHistoryDuration = 0.1f; // track last 0.1 seconds for velocity

    // Update the actual window position/size from world space
    void updateWindowTransform() const;
    Vec2 screenToWorld(const Vec2& screen) const;
    Vec2 worldToScreen(const Vec2& world) const;
};