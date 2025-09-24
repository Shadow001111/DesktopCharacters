#include "Character.h"

#include <iostream>

Vec2 Character::worldSize;   // The logical world size for physics
std::vector<Obstacle> Character::obstacles;

void Character::updateAABB()
{
    Vec2 halfSize = size * 0.5f;
    aabb = AABB(position - halfSize, position + halfSize);
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
    
    // Kinematics
    Vec2 gravity(0.0f, -10.0f);

    velocity += gravity * deltaTime;
    position += velocity * deltaTime;

    if (position.x < -(worldSize.x - size.x * 0.5f))
    {
        position.x = -(worldSize.x - size.x * 0.5f);
        velocity.x *= -0.9f;
    }
    else if (position.x > (worldSize.x - size.x * 0.5f))
    {
        position.x = (worldSize.x - size.x * 0.5f);
        velocity.x *= -0.9f;
    }

    if (position.y < -(worldSize.y - size.y * 0.5f))
    {
        position.y = -(worldSize.y - size.y * 0.5f);
        velocity.y *= -0.9f;
    }
    else if (position.y > (worldSize.y - size.y * 0.5f))
    {
        position.y = (worldSize.y - size.y * 0.5f);
        velocity.y *= -0.9f;
    }

    // Update AABB
    updateAABB();
}

const AABB& Character::getAABB() const
{
    return aabb;
}
