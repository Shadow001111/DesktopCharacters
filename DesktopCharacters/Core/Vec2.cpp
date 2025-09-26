#include "Vec2.h"
#include <cmath>

// Constructors
Vec2::Vec2() : x(0.0f), y(0.0f)
{
}

Vec2::Vec2(float x, float y) : x(x), y(y)
{
}

Vec2::Vec2(int x, int y) : x(static_cast<float>(x)), y(static_cast<float>(y))
{
}

// Arithmetic operators
Vec2 Vec2::operator+(const Vec2& other) const
{
    return Vec2(x + other.x, y + other.y);
}

Vec2 Vec2::operator-(const Vec2& other) const
{
    return Vec2(x - other.x, y - other.y);
}

Vec2 Vec2::operator*(const Vec2& other) const
{
    return Vec2(x * other.x, y * other.y);
}

Vec2 Vec2::operator/(const Vec2& other) const
{
    return Vec2(x / other.x, y / other.y);
}


Vec2 Vec2::operator+(float scalar) const
{
    return Vec2(x + scalar, y + scalar);
}

Vec2 Vec2::operator-(float scalar) const
{
    return Vec2(x - scalar, y - scalar);
}

Vec2 Vec2::operator*(float scalar) const
{
    return Vec2(x * scalar, y * scalar);
}

Vec2 Vec2::operator/(float scalar) const
{
    return Vec2(x / scalar, y / scalar);
}

Vec2 Vec2::operator-() const
{
    return Vec2(-x, -y);
}

// Assignment operators
Vec2& Vec2::operator+=(const Vec2& other)
{
    x += other.x;
    y += other.y;
    return *this;
}

Vec2& Vec2::operator-=(const Vec2& other)
{
    x -= other.x;
    y -= other.y;
    return *this;
}

Vec2& Vec2::operator*=(const Vec2& other)
{
    x *= other.x;
    y *= other.y;
    return *this;
}

Vec2& Vec2::operator/=(const Vec2& other)
{
    x /= other.x;
    y /= other.y;
    return *this;
}

Vec2& Vec2::operator+=(float scalar)
{
    x += scalar;
    y += scalar;
    return *this;
}

Vec2& Vec2::operator-=(float scalar)
{
    x -= scalar;
    y -= scalar;
    return *this;
}

Vec2& Vec2::operator*=(float scalar)
{
    x *= scalar;
    y *= scalar;
    return *this;
}

Vec2& Vec2::operator/=(float scalar)
{
    x /= scalar;
    y /= scalar;
    return *this;
}

// Comparison operators
bool Vec2::operator==(const Vec2& other) const
{
    return x == other.x && y == other.y;
}

bool Vec2::operator!=(const Vec2& other) const
{
    return !(*this == other);
}

// Utility methods
float Vec2::length() const
{
    return std::sqrt(x * x + y * y);
}

float Vec2::lengthSquared() const
{
    return x * x + y * y;
}

Vec2 Vec2::normalized() const
{
    float len = length();
    return len > 0 ? *this / len : Vec2();
}

void Vec2::normalize()
{
    *this = normalized();
}

// Static utility methods
float Vec2::distance(const Vec2& a, const Vec2& b)
{
    return (b - a).length();
}

float Vec2::distanceSquared(const Vec2& a, const Vec2& b)
{
    return (b - a).lengthSquared();
}

Vec2 Vec2::lerp(const Vec2& a, const Vec2& b, float t)
{
    return a + (b - a) * t;
}