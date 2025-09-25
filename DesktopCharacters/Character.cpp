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
    
    for (const auto& obstacle : obstacles)
    {
        if (obstacle.type == ObstacleType::Horizontal)
        {
            // X check
            float obstX1 = obstacle.min;
            float obstX2 = obstacle.max;

            float charX1 = position.x - size.x * 0.5f;
            float charX2 = position.x + size.x * 0.5f;

            if (!(obstX1 < charX2 && charX1 < obstX2))
            {
                continue;
            }

            // Y check
            float sign = copysignf(1.0f, velocity.y);
            float t = (obstacle.perpOffset - (position.y + size.y * 0.5f * sign)) / velocity.y;
            if (!(t <= 0.0f || t > deltaTime) && t < minTime)
            {
                minTime = t;
                flipVelocityXorY = true;
            }
        }
        else
        {
            // Y check
            float obstY1 = obstacle.min;
            float obstY2 = obstacle.max;

            float charY1 = position.y - size.y * 0.5f;
            float charY2 = position.y + size.y * 0.5f;

            if (!(obstY1 < charY2 && charY1 < obstY2))
            {
                continue;
            }

            // X check
            float sign = copysignf(1.0f, velocity.x);
            float t = (obstacle.perpOffset - (position.x + size.x * 0.5f * sign)) / velocity.x;
            if (!(t <= 0.0f || t > deltaTime) && t < minTime)
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
            velocity.y *= -1.0f;
        }
        else
        {
            velocity.x *= -1.0f;
        }
        return minTime;
    }
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

void Character::setSize(float width, float height)
{
    setSize(Vec2(width, height));
}

void Character::setSize(const Vec2& newSize)
{
    size = newSize;
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
