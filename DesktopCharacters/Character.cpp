#include "Character.h"

#include <iostream>

Vec2 Character::worldSize;   // The logical world size for physics


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
    if (dragging)
    {
        velocity = Vec2();

        //int mouseX, mouseY;
        //platform->getGlobalMousePosition(mouseX, mouseY);

        //Vec2 mousePosInWorldSpace = screenToWorld(Vec2(mouseX, mouseY));

        //position = mousePosInWorldSpace + dragOffset;

        //// Record history
        //dragHistory.push_back({ position, deltaTime });
        //float totalTime = 0.0f;
        //for (int i = (int)dragHistory.size() - 1; i >= 0; --i)
        //{
        //    totalTime += dragHistory[i].time;
        //    if (totalTime > dragHistoryDuration)
        //    {
        //        dragHistory.erase(dragHistory.begin(), dragHistory.begin() + i);
        //        break;
        //    }
        //}
        return;
    }
        
    /*dragging = false;
    if (!dragHistory.empty())
    {
        Vec2 deltaPos = dragHistory.back().position - dragHistory.front().position;
        float totalTime = 0.0f;
        for (auto& s : dragHistory) totalTime += s.time;

        if (totalTime > 0.0f)
            velocity = deltaPos / totalTime;
    }
    dragHistory.clear();*/
    
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
}

void Character::startDrag(const Vec2& mousePos)
{
    dragging = true;
    dragOffset = position - mousePos;

    dragHistory.clear();
    dragHistory.push_back({ position, 0.0f });
}
