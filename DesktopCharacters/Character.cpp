#include "Character.h"

#include <iostream>

Vec2 Character::worldSize;   // The logical world size for physics
std::vector<Obstacle> Character::obstacles;

void Character::updateAABB()
{
    Vec2 halfSize = size * 0.5f;
    aabb = AABB(position - halfSize, position + halfSize);
}

float Character::collisions(float deltaTime)
{
    float minTime = FLT_MAX;
    bool flipVelocityXorY = false;

    const Vec2 halfSize = size * 0.5f;

    const float charX1 = position.x - halfSize.x;
    const float charX2 = position.x + halfSize.x;
    const float charY1 = position.y - halfSize.y;
    const float charY2 = position.y + halfSize.y;

    const float signVelX = copysignf(1.0f, velocity.x);
    const float signVelY = copysignf(1.0f, velocity.y);

    const float charBorderX = position.x + halfSize.x * signVelX;
    const float charBorderY = position.y + halfSize.y * signVelY;
    
    for (const auto& obstacle : obstacles)
    {
        if (obstacle.type == ObstacleType::Horizontal)
        {
            if (velocity.y == 0.0f)
            {
                continue;
            }

            // X check
            if (!collisionAxisCheck(charX1, charX2, obstacle))
            {
                continue;
            }

            // Y check
            float t = (obstacle.perpOffset - charBorderY) / velocity.y;
            if (t >= 0.0f && t <= deltaTime && t < minTime)
            {
                minTime = t;
                flipVelocityXorY = true;
            }
        }
        else
        {
            if (velocity.x == 0.0f)
            {
                continue;
            }

            // Y check
            if (!collisionAxisCheck(charY1, charY2, obstacle))
            {
                continue;
            }

            // X check
            float t = (obstacle.perpOffset - charBorderX) / velocity.x;
            if (t >= 0.0f && t <= deltaTime && t < minTime)
            {
                minTime = t;
                flipVelocityXorY = false;
            }
        }
    }

    if (minTime > 999.0f)
    {
        position += velocity * deltaTime;
        return 0.0f;
    }
    else
    {
        position += velocity * minTime;
        if (flipVelocityXorY)
        {
            velocity.y *= -elastcity;
        }
        else
        {
            velocity.x *= -1.0f; // elastcity;
        }
        return deltaTime - minTime;
    }
}

bool Character::collisionAxisCheck(float axisMin, float axisMax, const Obstacle& obstacle)
{
    for (const auto& segment : obstacle.segments)
    {
        float obstX1 = segment.min;
        float obstX2 = segment.max;

        if (obstX1 < axisMax && axisMin < obstX2)
        {
            return true;
        }
    }
    return false;
}

Character::Character(const Vec2& position, const Vec2& size)
    : position(position), size(size), velocity()
{
}

Character::~Character()
{
}

void Character::setPosition(float x, float y)
{
    setPosition(Vec2(x, y));
}

void Character::setPosition(const Vec2& newPosition)
{
    position = newPosition;
}

void Character::setVelocity(float vx, float vy)
{
    setVelocity(Vec2(vx, vy));
}

void Character::setVelocity(const Vec2& newVelocity)
{
    velocity = newVelocity;
}

void Character::move(float deltaX, float deltaY)
{
    move(Vec2(deltaX, deltaY));
}

void Character::move(const Vec2& delta)
{
    setPosition(position + delta);
}

const Vec2& Character::getPosition()
{
    return position;
}

const Vec2& Character::getSize()
{
    return size;
}

const Vec2& Character::getVelocity()
{
    return velocity;
}

void Character::update(float deltaTime)
{
    // Dragging
    if (isBeingDragged)
    {
        velocity = Vec2();
        updateAABB();
        return;
    }
    
    // Kinematics / Collisions
    Vec2 gravity(0.0f, -10.0f);
    velocity += gravity * deltaTime;

    float timeBudget = deltaTime;
    while (timeBudget > 0.0f)
    {
        timeBudget = collisions(timeBudget);
    }

    // Update AABB
    updateAABB();
}

const AABB& Character::getAABB() const
{
    return aabb;
}

Obstacle::Segment::Segment(float min, float max) :
    min(min), max(max)
{
}

Obstacle::Obstacle() :
    type(ObstacleType::Horizontal)
{
}

Obstacle::Obstacle(ObstacleType type, float perpOffset, float minX, float maxX) :
    type(type), perpOffset(perpOffset)
{
    segments.emplace_back(minX, maxX);
}

Obstacle::Obstacle(Obstacle&& other) noexcept :
    type(other.type), perpOffset(other.perpOffset), segments(std::move(other.segments))
{
}

Obstacle& Obstacle::operator=(Obstacle&& other) noexcept
{
    if (this != &other)
    {
        type = other.type;
        perpOffset = other.perpOffset;
        segments = std::move(other.segments);
    }
    return *this;
}
