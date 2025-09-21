#pragma once
#include "Window/BaseWindow.h"

#include "Core/Vec2.h"

#include <memory>

// Character represents a desktop character entity
class Character
{
public:
    Character();
    ~Character();

    // Create the character with a window
    bool create(const WindowParams& params);

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
    void updateDrag(const Vec2& mousePos);
    void endDrag();
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

    // Update the actual window position/size from world space
    void updateWindowTransform();
};