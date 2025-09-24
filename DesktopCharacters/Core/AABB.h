#pragma once
#include "Vec2.h"

struct AABB
{
	float minX, minY, maxX, maxY;

	AABB();
	AABB(float minX, float minY, float maxX, float maxY);
	AABB(const Vec2& min, const Vec2& max);

	bool isIntersecting(const AABB& other) const;
	bool isContaining(const Vec2& point) const;
};

