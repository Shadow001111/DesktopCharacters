#pragma once
#undef min
#undef max

#include "Core/AABB.h"

#include "Obstacle.h"

#include <vector>

// Character represents a desktop character entity
class Character
{
public:
    struct Data
    {
        float maxSpeed = 0.0f;
        float maxJumpVelocity = 0.0f;

        // TODO: Elasticity isn't movement-related, so rename class.
        float collisionElasticitySides = 0.0f;
        float collisionElasticityRoof = 0.0f;
        float collisionElasticityFloor = 0.0f;

        float frictionFloor = 0.0f;
    };
private:
    Vec2 position;
    Vec2 size;
    Vec2 velocity;

    AABB aabb;
    Data data;

    bool isGrounded = false;

    bool collisionAxisCheck(float axisMin, float axisMax, const Obstacle& obstacle) const;
    float collisions(float deltaTime);
public:
    static Vec2 worldSize; // Center is at zero
    static std::vector<Obstacle> obstacles;

    bool isBeingDragged = false;

    Character(const Vec2& position, const Vec2& size, const Data& data);
    ~Character();

    //
    void update(float deltaTime);
    void updateAABB();

    //
    void setPosition(float x, float y);
    void setPosition(const Vec2& newPosition);
    void setVelocity(float vx, float vy);
    void setVelocity(const Vec2& newVelocity);
    void move(float deltaX, float deltaY);
    void move(const Vec2& delta);

    // Getters
    const Vec2& getPosition() const;
    const Vec2& getSize() const;
    const Vec2& getVelocity() const;
    const AABB& getAABB() const;
};