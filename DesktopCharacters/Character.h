#pragma once
#include "Core/Vec2.h"

#include <memory>
#include <vector>

// Character represents a desktop character entity
class Character
{
public:
    static Vec2 worldSize; // Center is at zero

    Character(const Vec2& position, const Vec2& size);
    ~Character();

    void setPosition(float x, float y);
    void setPosition(const Vec2& newPosition);

    void setSize(float width, float height);
    void setSize(const Vec2& newSize);

    void setVelocity(float vx, float vy);
    void setVelocity(const Vec2& newVelocity);

    void move(float deltaX, float deltaY);
    void move(const Vec2& delta);

    const Vec2& getPosition();
    const Vec2& getSize();
    const Vec2& getVelocity();

    void update(float deltaTime);

    void startDrag(const Vec2& mousePos);
private:
    // Character position and size
    Vec2 position;
    Vec2 size;
    Vec2 velocity;

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
};