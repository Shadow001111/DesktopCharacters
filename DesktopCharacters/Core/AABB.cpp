#include "AABB.h"

AABB::AABB() :
	minX(0), minY(0), maxX(0), maxY(0)
{
}

AABB::AABB(float minX, float minY, float maxX, float maxY) :
	minX(minX), minY(minY), maxX(maxX), maxY(maxY)
{
}

AABB::AABB(const Vec2& min, const Vec2& max) :
	minX(min.x), minY(min.y), maxX(max.x), maxY(max.y)
{
}

bool AABB::isIntersecting(const AABB& other) const
{
	return minX < other.maxX && maxX > other.minX && minY < other.maxY && maxY > other.minY;
}

bool AABB::isContaining(const Vec2& point) const
{
	return minX <= point.x && point.x <= maxX && minY <= point.y && point.y <= maxY;
}

bool AABB::isContaining(const AABB& inner) const
{
	return inner.minX >= minX && inner.maxX <= maxX && inner.minY >= minY && inner.maxY <= maxY;
}
