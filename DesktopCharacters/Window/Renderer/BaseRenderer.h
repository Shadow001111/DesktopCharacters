#pragma once
#include "Core/Color.h"
#include "Core/Vec2.h"

class BaseRenderer
{
public:
	virtual ~BaseRenderer() = default;

	virtual void beforeRender() = 0;
	virtual void afterRender() = 0;

	// Shape drawing
	virtual void drawRectangle(float x, float y, float w, float h, const Color& color) = 0;
	void drawRectangle(const Vec2& position, const Vec2& size, const Color& color);

	virtual void drawEllipse(float cx, float cy, float rx, float ry, const Color& color) = 0;
	void drawEllipse(const Vec2& center, const Vec2& radius, const Color& color);

	virtual void drawLine(float x1, float y1, float x2, float y2, const Color& color, float strokeWidth = 2.0f) = 0;
	void drawLine(const Vec2& start, const Vec2& end, const Color& color, float strokeWidth = 2.0f);
};

