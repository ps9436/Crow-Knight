#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Character.hpp" 
#include "Animation.hpp"

class Projectile {
protected:
    Vector2 position;
    Vector2 velocity;
    float radius;
    float rotation = 0.0f;
    Animation animation;

public:
    bool active = true;

    Projectile(Vector2 pos, Vector2 target, float speed, float rad, float rotationOffset = 0.0f) 
        : position(pos), radius(rad) {
        
        // Calculate Direction & Velocity once
        Vector2 dir = Vector2Subtract(target, pos);
        dir = Vector2Normalize(dir);
        velocity = Vector2Scale(dir, speed);

        // Calculate Rotation (angle in degrees)
        rotation = atan2(dir.y, dir.x) * RAD2DEG;
    }

    virtual ~Projectile() = default;

    virtual void Update(float dt) {
        // Physics
        position = Vector2Add(position, Vector2Scale(velocity, dt));
        
        // Animation
        animation.Update(dt);

        // If too far, remove
        if (position.x < -2000 || position.x > 5000 || 
            position.y < -2000 || position.y > 5000) {
            active = false;
        }
    }

    virtual void Draw() {
        animation.Draw(position, true, rotation);
    }

    virtual void OnHit(Character& target) = 0; 
    
    // Getters for collision
    Vector2 GetPosition() const { return position; }
    float GetRadius() const { return radius; }
};