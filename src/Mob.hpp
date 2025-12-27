#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Character.hpp"

class Mob : public Character {
    public:
        float speed;
        int health;
        int damageCounter = 0;  // Stun

        float deadTimer = 0.0f;

        // Radius for collision/spacing
        float radius = 20.0f;

        // Spawn logic
        // Spawn enemy at a random spot outside camera view
        void Spawn(Vector2 playerPos, int screenW, int screenH) {
            // Reset stats
            deadTimer = 0.0f;
            currentState = CharacterState::RUN;

            // Randomly choose an edge
            int side = GetRandomValue(0,3);
            int buffer = 100;   // Spawn 100px outside screen

            switch(side) {
                case 0: // Left
                    position = { playerPos.x - screenW/2 - buffer, playerPos.y };
                    break;
                case 1: // Right
                    position = { playerPos.x + screenW/2 + buffer, playerPos.y };
                    break;
                case 2: // Top
                    position = { playerPos.x, playerPos.y - screenH/2 - buffer };
                    break;
                case 3: // Bottom
                    position = { playerPos.x, playerPos.y + screenH/2 + buffer };
                    break;
            }
        }
        
        void Update(Vector2 playerPos, bool playerAttacking, Rectangle playerHitbox, Vector2 pushForce) {
            float dt = GetFrameTime();

            // Update the animation
            if (animations.count(currentState) > 0) {
                animations[currentState].Update(dt);
            }

            // Death logic
            if (currentState == CharacterState::DEATH) {
                deadTimer += 0.016f;
                return;     // Don't move if dead
            }
            // Hurt logic
            if (currentState == CharacterState::HURT) {
                damageCounter++;
                position.x--;
                position.y--;
                // Stunned for 20 frames, then return to run
                if (damageCounter >= 20) {
                    currentState = CharacterState::RUN;
                    damageCounter = 0;
                }
                return;     // Don't move while hurt
            }
            // Movement logic (run towards player)
            if (currentState == CharacterState::RUN) {
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

            if (playerAttacking && CheckCollisionRecs(playerHitbox, myHitbox)) {
                TakeDamage();
            }
        }

        void TakeDamage() {
            if (currentState == CharacterState::HURT || currentState == CharacterState::DEATH) return;

            health--;
            if (health <= 0) {
                currentState = CharacterState::DEATH;
                // Reset death animation to start from frame 0
                if (animations.count(CharacterState::DEATH)) {
                    animations[CharacterState::DEATH].Reset();
                }
            } else {
                currentState = CharacterState::HURT;
                damageCounter = 0;
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