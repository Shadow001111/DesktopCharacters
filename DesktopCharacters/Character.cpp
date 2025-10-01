#include "Character.h"

#include <iostream>

Vec2 Character::worldSize;   // The logical world size for physics
std::vector<Obstacle> Character::obstacles;

// Checks overlap between a character's axis range and obstacle segments
bool Character::collisionAxisCheck(float axisMin, float axisMax, const Obstacle& obstacle, size_t& returnSegmentIndex) const
{
    returnSegmentIndex = 0;
    for (const auto& segment : obstacle.segments)
    {
        float obstX1 = segment.min;
        float obstX2 = segment.max;

        if (obstX1 < axisMax && axisMin < obstX2)
        {
            return true;
        }

        returnSegmentIndex++;
    }
    return false;
}

// Handles collisions and movement during deltaTime
// Returns leftover time if a collision occurs
float Character::collisions(float deltaTime)
{
    float minimalTimeUntilCollision = FLT_MAX;
    const Obstacle* closestObstacle = nullptr;
    size_t closestSegmentIndex = 0;

    const Vec2 halfSize = size * 0.5f;

    // Character bounds
    const float charX1 = position.x - halfSize.x;
    const float charX2 = position.x + halfSize.x;
    const float charY1 = position.y - halfSize.y;
    const float charY2 = position.y + halfSize.y;

    // Velocity direction (sign)
    const float signVelX = copysignf(1.0f, velocity.x);
    const float signVelY = copysignf(1.0f, velocity.y);

    // Character border in movement direction
    const float charBorderX = position.x + halfSize.x * signVelX;
    const float charBorderY = position.y + halfSize.y * signVelY;
    
    for (const auto& obstacle : obstacles)
    {
        size_t segmentIndex = 0;

        if (obstacle.type == Obstacle::Type::Horizontal)
        {
            if (velocity.y == 0.0f)
            {
                continue;
            }

            // X overlap check
            if (!collisionAxisCheck(charX1, charX2, obstacle, segmentIndex))
            {
                continue;
            }

            // Compute time until collision in Y
            float t = (obstacle.perpOffset - charBorderY) / velocity.y;
            if (t >= 0.0f && t <= deltaTime && t < minimalTimeUntilCollision)
            {
                minimalTimeUntilCollision = t;
                closestObstacle = &obstacle;
                closestSegmentIndex = segmentIndex;
            }
        }
        else
        {
            if (velocity.x == 0.0f)
            {
                continue;
            }

            // Y overlap check
            if (!collisionAxisCheck(charY1, charY2, obstacle, segmentIndex))
            {
                continue;
            }

            // Compute time until collision in X
            float t = (obstacle.perpOffset - charBorderX) / velocity.x;
            if (t >= 0.0f && t <= deltaTime && t < minimalTimeUntilCollision)
            {
                minimalTimeUntilCollision = t;
                closestObstacle = &obstacle;
                closestSegmentIndex = segmentIndex;
            }
        }
    }

    if (closestObstacle == nullptr)
    {
        // No collision within deltaTime
        position += velocity * deltaTime;
        return 0.0f;
    }
    else
    {
        // Collision detected
        position += velocity * minimalTimeUntilCollision;
        if (closestObstacle->type == Obstacle::Type::Horizontal)
        {
            float elasticity = signVelY > 0.0f ? data.collisionElasticityRoof : data.collisionElasticityFloor;
            velocity.y *= -elasticity;
            groundedData.isGrounded = signVelY < 0.0f;
        }
        else
        {
            velocity.x *= -data.collisionElasticitySides;
            groundedData.isGrounded = false;
        }
        groundedData.segmentIndex = closestSegmentIndex;

        return deltaTime - minimalTimeUntilCollision; // Remaining time to process
    }
}


Character::Character(const Vec2& position, const Vec2& size, const Data& data)
    : position(position), size(size), velocity(), data(data)
{
}

Character::~Character()
{
}


void Character::update(float deltaTime)
{
    groundedData.isGrounded = false;
    isMovingPurposefully = false;

    // If dragged by user -> stop physics
    if (isBeingDragged)
    {
        velocity = Vec2();
        updateAABB();
        return;
    }

    // Apply gravity
    // TODO: Make it world setting
    Vec2 gravity(0.0f, -20.0f);
    velocity += gravity * deltaTime;

    // Process collisions within the time step
    float timeBudget = deltaTime;
    while (timeBudget > 0.0f)
    {
        timeBudget = collisions(timeBudget);
    }

    // Follow target
    if (groundedData.isGrounded && fabsf(velocity.x) <= data.maxSpeed)
    {
        followTarget(deltaTime);
    }

    if (!isMovingPurposefully)
    {
        // Friction with floor
        if (groundedData.isGrounded)
        {
            const float signVelX = copysignf(1.0f, velocity.x);
            float frictionImpulse = data.frictionFloor;

            if (frictionImpulse > fabsf(velocity.x))
            {
                velocity.x = 0.0f;
            }
            else
            {
                velocity.x -= frictionImpulse * signVelX;
            }
        }
    }

    // Refresh bounding box after movement
    updateAABB();
}

void Character::updateAABB()
{
    Vec2 halfSize = size * 0.5f;
    aabb = AABB(position - halfSize, position + halfSize);
}

void Character::followTarget(float deltaTime)
{
    if (!targetToFollow.exist)
    {
        return;
    }

    isMovingPurposefully = true;

    float dpos = targetToFollow.position.x - position.x;
    float sign = copysignf(1.0f, dpos);

    float speed = fabsf(dpos / deltaTime);
    speed = fminf(speed, data.maxSpeed);

    velocity.x = speed * sign;
}


void Character::setFollowTarget(const FollowTarget& newTarget)
{
    targetToFollow = newTarget;
}


void Character::setPosition(float x, float y)
{
    setPosition(Vec2(x, y));
}

void Character::setPosition(const Vec2& newPosition)
{
    position = newPosition;
}

void Character::setVelocity(float vx, float vy)
{
    setVelocity(Vec2(vx, vy));
}

void Character::setVelocity(const Vec2& newVelocity)
{
    velocity = newVelocity;
}

// Moves character relative to current position
void Character::move(float deltaX, float deltaY)
{
    move(Vec2(deltaX, deltaY));
}

// Moves character relative to current position
void Character::move(const Vec2& delta)
{
    setPosition(position + delta);
}


const Vec2& Character::getPosition() const
{
    return position;
}

const Vec2& Character::getSize() const
{
    return size;
}

const Vec2& Character::getVelocity() const
{
    return velocity;
}

const AABB& Character::getAABB() const
{
    return aabb;
}
