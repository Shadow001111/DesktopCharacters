#pragma once

// 2D Vector class for handling positions, velocities, and sizes
class Vec2
{
public:
    float x, y;

    // Constructors
    Vec2();
    Vec2(float x, float y);
    Vec2(int x, int y);

    // Arithmetic operators
    Vec2 operator+(const Vec2& other) const;
    Vec2 operator-(const Vec2& other) const;
    Vec2 operator*(const Vec2& other) const;
    Vec2 operator/(const Vec2& other) const;

    Vec2 operator+(float scalar) const;
    Vec2 operator-(float scalar) const;
    Vec2 operator*(float scalar) const;
    Vec2 operator/(float scalar) const;

    Vec2 operator-() const;

    // Assignment operators
    Vec2& operator+=(const Vec2& other);
    Vec2& operator-=(const Vec2& other);
    Vec2& operator*=(const Vec2& other);
    Vec2& operator/=(const Vec2& other);

    Vec2& operator+=(float scalar);
    Vec2& operator-=(float scalar);
    Vec2& operator*=(float scalar);
    Vec2& operator/=(float scalar);

    // Comparison operators
    bool operator==(const Vec2& other) const;
    bool operator!=(const Vec2& other) const;

    // Utility methods
    float length() const;
    float lengthSquared() const;
    Vec2 normalized() const;
    void normalize();

    // Static utility methods
    static float distance(const Vec2& a, const Vec2& b);
    static float distanceSquared(const Vec2& a, const Vec2& b);
    static Vec2 lerp(const Vec2& a, const Vec2& b, float t);
};