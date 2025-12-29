#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Character.hpp"

class Mob : public Character {
    public:
        float speed;
        int health;
        int damageCounter = 0;  // Stun
        Vector2 knockback = { 0.0f, 0.0f };
        float weight = 1.0f;    // < 1.0 heavy, > 1.0 light

        float immunityTimer = 0.0f;
        float deadTimer = 0.0f;

        // Radius for collision/spacing
        float radius = 20.0f;
        Vector2 pushForce = {0, 0}; // Force on each mob (chase and collisions)

        // Spawn logic
        // Spawn enemy at a random spot outside camera view
        void Spawn(Vector2 playerPos, int screenW, int screenH) {
            // Reset
            deadTimer = 0.0f;
            currentState = CharacterState::RUN;

            // Edges of the camera view
            float leftEdge   = playerPos.x - (screenW / 2);
            float rightEdge  = playerPos.x + (screenW / 2);
            float topEdge    = playerPos.y - (screenH / 2);
            float bottomEdge = playerPos.y + (screenH / 2);

            int buffer = GetRandomValue(-50, 10); // Distance outside the screen
            int side = GetRandomValue(0, 3);

            switch(side) {
                case 0: // Left Wall
                    // X is fixed, Y is Random (Top to Bottom)
                    position.x = leftEdge - buffer;
                    position.y = (float)GetRandomValue((int)topEdge, (int)bottomEdge); 
                    break;

                case 1: // Right Wall
                    // X is fixed, Y is Random (Top to Bottom)
                    position.x = rightEdge + buffer;
                    position.y = (float)GetRandomValue((int)topEdge, (int)bottomEdge);
                    break;

                case 2: // Top Wall
                    // X is Random (Left to Right), Y is Fixed
                    position.x = (float)GetRandomValue((int)leftEdge, (int)rightEdge);
                    position.y = topEdge - buffer;
                    break;

                case 3: // Bottom Wall
                    // X is Random (Left to Right), Y is Fixed
                    position.x = (float)GetRandomValue((int)leftEdge, (int)rightEdge);
                    position.y = bottomEdge + buffer;
                    break;
            }
        }
        
        void Update(Vector2 playerPos, bool playerAttacking, Rectangle playerHitbox, Vector2 pushForce, float* globalHitStop, float dt) {
            // float dt = GetFrameTime();

            // Update the animation
            if (animations.count(currentState) > 0) animations[currentState].Update(dt);
            // Decrement  immune time
            if (immunityTimer > 0.0f) immunityTimer -= dt;

            // Death logic
            if (currentState == CharacterState::DEATH) {
                deadTimer += dt;
                damageCounter++;
                
                // Apply knockback
                position = Vector2Add(position, Vector2Scale(knockback, dt));
                // Apply Friction (multiply by a value < 1.0 each frame to slow down)
                float friction = 0.90f;
                if (weight < 1.0f) friction = 0.80f;
                knockback = Vector2Scale(knockback, friction);

                // Shake effect
                if (deadTimer < 0.2f) {
                    if (damageCounter % 4 < 2) position.x -= 5; 
                    else position.x += 5;
                }
                return;     // Don't need other checks
            }
            // Hurt logic
            if (currentState == CharacterState::HURT) {
                damageCounter++;

                // Apply knockback
                position = Vector2Add(position, Vector2Scale(knockback, dt));

                // Apply Friction (multiply by a value < 1.0 each frame to slow down)
                float friction = 0.90f;
                if (weight < 1.0f) friction = 0.80f;
                knockback = Vector2Scale(knockback, friction);

                // Shake effect
                if (damageCounter % 4 < 2) position.x -= 5; 
                else position.x += 5;

                // Stunned for 20 frames, then return to run
                if (damageCounter >= 20) {
                    currentState = CharacterState::RUN;
                    damageCounter = 0;
                    knockback = {0,0};
                }
            }
            // Movement logic (run towards player)
            else if (currentState == CharacterState::RUN) {
                Vector2 direction = Vector2Subtract(playerPos, position);
                direction = Vector2Add(direction, pushForce);   // 

                // Normalize for direction
                if (Vector2Length(direction) > 1.0f) {
                    direction = Vector2Normalize(direction);
                    float pps = speed * dt;     // pixel per second
                    position = Vector2Add(position, Vector2Scale(direction, pps));
                }
                
                // Update facing
                faceRight = (playerPos.x > position.x);
            }

            // Check collisions
            Rectangle myHitbox = GetHitbox();

            if (playerAttacking && immunityTimer <= 0.0f && CheckCollisionRecs(playerHitbox, myHitbox)) {
                TakeDamage(playerPos);
                // if (health <= 0) *globalHitStop = 0.1f; // Freeze for kill
            }
        }

        void TakeDamage(Vector2 playerPos) {
            if (currentState == CharacterState::DEATH) return;

            health--;
            immunityTimer = 0.15f;

            // Knockback direction
            Vector2 pushDir = Vector2Subtract(position, playerPos);
            pushDir = Vector2Normalize(pushDir);
            // Knockback force
            knockback = Vector2Scale(pushDir, 800.0f * weight);

            if (health <= 0) {
                currentState = CharacterState::DEATH;
                // Reset death animation to start from frame 0
                if (animations.count(CharacterState::DEATH)) {
                    animations[CharacterState::DEATH].Reset();
                }
            } else {
                currentState = CharacterState::HURT;
                damageCounter = 0;
                if (animations.count(CharacterState::HURT)) {
            animations[CharacterState::HURT].Reset();
                }
            }
        }

        virtual void Draw() override {
            Character::Draw();
        }

        bool IsDeadAndGone() {
            if (currentState != CharacterState::DEATH) return false;
            
            // Check if the death animation has finished playing
            if (animations.count(CharacterState::DEATH)) {
                Animation& anim = animations[CharacterState::DEATH];
                return anim.currentFrame >= anim.totalFrames - 1;
            }
            return true; // If no animation exists, remove instantly
        }
};