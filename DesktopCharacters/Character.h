#pragma once
#include "Core/AABB.h"

#include <memory>
#include <vector>

// Character represents a desktop character entity
class Character
{
    Vec2 position;
    Vec2 size;
    Vec2 velocity;

    AABB aabb;

    void updateAABB();
public:
    static Vec2 worldSize; // Center is at zero

    bool isBeingDragged = false;

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

    const AABB& getAABB() const;
};