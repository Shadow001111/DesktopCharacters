#pragma once
#include <vector>

struct Obstacle
{
    enum class Type : char { Horizontal, Vertical };

    struct Segment
    {
        float min = 0.0f, max = 0.0f;

        Segment() = default;
        Segment(float min, float max);
    };

    Type type;
    float perpOffset = 0.0f;
    std::vector<Segment> segments;

    Obstacle();
    Obstacle(Type type, float perpOffset, float minX, float maxX);

    Obstacle(const Obstacle& other) = default;
    Obstacle& operator=(const Obstacle& other) = default;

    Obstacle(Obstacle&& other) noexcept;
    Obstacle& operator=(Obstacle&& other) noexcept;
};