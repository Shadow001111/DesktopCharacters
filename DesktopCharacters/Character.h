#pragma once
#include "Core/AABB.h"

#include <memory>
#include <vector>

enum class ObstacleType
{
    Horizontal, Vertical
};

struct Obstacle
{
    struct ObstacleSegment
    {
        float min = 0.0f, max = 0.0f;
    };
    ObstacleType type;
    float perpOffset = 0.0f;
    std::vector<ObstacleSegment> segments;
};

// Character represents a desktop character entity
class Character
{
    Vec2 position;
    Vec2 size;
    Vec2 velocity;

    AABB aabb;

    float elastcity = 0.9f;

    void updateAABB();

    float collisions(float deltaTime);
    bool collisionAxisCheck(float axisMin, float axisMax, const Obstacle& obstacle);
public:
    static Vec2 worldSize; // Center is at zero
    static std::vector<Obstacle> obstacles;

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