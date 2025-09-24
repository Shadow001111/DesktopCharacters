#include "BaseRenderer.h"

void BaseRenderer::drawRectangle(const Vec2& position, const Vec2& size, const Color& color)
{
	drawRectangle(position.x, position.y, size.x, size.y, color);
}

void BaseRenderer::drawEllipse(const Vec2& center, const Vec2& radius, const Color& color)
{
	drawEllipse(center.x, center.y, radius.x, radius.y, color);
}

void BaseRenderer::drawLine(const Vec2& start, const Vec2& end, const Color& color, float strokeWidth)
{
	drawLine(start.x, start.y, end.x, end.y, color, strokeWidth);
}
