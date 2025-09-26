#include "Obstacle.h"

Obstacle::Segment::Segment(float min, float max) :
    min(min), max(max)
{
}

Obstacle::Obstacle() :
    type(Type::Horizontal)
{
}

Obstacle::Obstacle(Type type, float perpOffset, float minX, float maxX) :
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