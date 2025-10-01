#pragma once
#include "Core/Range.h"
#include "Core/Vec2.h"

#include <vector>

struct Obstacle
{
    enum class Type : char { Horizontal, Vertical };

    /*struct Single
    {
        Type type;
        float perpOffset;
        Range segment;
    };*/

    Type type;
    float perpOffset = 0.0f;
    std::vector<Range> segments;
    Vec2 velocity = { 0.0f, 0.0f };

    Obstacle();
    Obstacle(Type type, float perpOffset, float minX, float maxX);

    Obstacle(const Obstacle& other) = default;
    Obstacle& operator=(const Obstacle& other) = default;

    Obstacle(Obstacle&& other) noexcept;
    Obstacle& operator=(Obstacle&& other) noexcept;
};