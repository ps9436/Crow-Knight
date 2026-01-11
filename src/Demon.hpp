#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Mob.hpp"

class Demon : public Mob {
    private:
        Animation demonRUN;
        Animation demonHURT;
        Animation demonDEATH;
        Animation demonATTACK;
        Animation demonDASH;

        bool hasTeleported = false; // Only teleport once

    public:
        // Flag for Game
        bool exploded = false;
        bool dealtDamage = false;
        float explosionRad = 80.0f;
        float damageInstant = 50.0f;

        static inline Texture2D runDemon;
        static inline Texture2D hurtDemon;
        static inline Texture2D deadDemon;
        static inline Texture2D attackDemon;
        static inline Texture2D dashDemon;
        static inline Texture2D shadowDemon;

        Demon () { this->shadow = shadowDemon; }

        static void StaticLoad() {
            Demon::runDemon = LoadTexture("assets/demon/demon-run-Sheet.png");
            Demon::hurtDemon = LoadTexture("assets/demon/demon-hurt-Sheet.png");
            Demon::deadDemon = LoadTexture("assets/demon/demon-death-Sheet.png");
            Demon::attackDemon = LoadTexture("assets/demon/demon-attack-Sheet.png");
            Demon::dashDemon = LoadTexture("assets/demon/demon-dash-Sheet.png");
            Demon::shadowDemon = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runDemon);
            UnloadTexture(hurtDemon);
            UnloadTexture(deadDemon);
            UnloadTexture(attackDemon);
            UnloadTexture(dashDemon);
            UnloadTexture(shadowDemon);
        }

        void init(int scale = 4) {
            size = scale;
            speed = 275.0f;
            health = 3;
            weight = 1.0f;
            radius = 25.0f;
            damageRate = 20.0f; 
            lifestealRate = 5.0f;

            demonRUN.Init(runDemon, 4, 8.0f, scale);
            animations[CharacterState::RUN] = demonRUN;
            
            demonHURT.Init(hurtDemon, 4, 12.0f, scale);
            animations[CharacterState::HURT] = demonHURT;

            demonDEATH.Init(deadDemon, 8, 12.0f, scale, false);
            animations[CharacterState::DEATH] = demonDEATH;

            demonATTACK.Init(attackDemon, 15, 12.0f, scale, false); 
            animations[CharacterState::ATTACK_SIDE] = demonATTACK;

            demonDASH.Init(dashDemon, 11, 24.0f, scale, false); 
            animations[CharacterState::DASH] = demonDASH;

            animations[CharacterState::IDLE] = demonRUN;
            currentState = CharacterState::RUN;
        }

        void Update(Vector2 playerPos, bool playerAttacking, Rectangle playerHitbox, Vector2 pushForce, float* globalHitStop, float dt) override {

            if (currentState == CharacterState::HURT || currentState == CharacterState::DEATH) {
                Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);
                return; // Do not run rest
            }

            // Teleport when HP is 1
            if (health <= 1 && !hasTeleported) {
                hasTeleported = true;
    
                // Enter Dash
                currentState = CharacterState::DASH;
                animations[CharacterState::DASH].Reset();
            }
            
            if (currentState == CharacterState::DASH) {
                if (animations[CharacterState::DASH].IsFinished()) {
                    // Teleport behind player
                    Vector2 dirToDemon = Vector2Subtract(position, playerPos);
                    dirToDemon = Vector2Normalize(dirToDemon);
                    Vector2 dirBehind = Vector2Negate(dirToDemon);  // Invert for opposite side
                    position = Vector2Add(playerPos, Vector2Scale(dirBehind, 50.0f));   // 50.0f is offset

                    // Enter Explosion Mode
                    currentState = CharacterState::ATTACK_SIDE;
                    animations[CharacterState::ATTACK_SIDE].Reset();
                }
            }
            else if (currentState == CharacterState::ATTACK_SIDE) {
                // Exploding
                if (exploded) animations[CharacterState::ATTACK_SIDE].Update(dt);

                // Explosion after frame 6
                if (animations[CharacterState::ATTACK_SIDE].currentFrame > 6) {
                    exploded = true;
                    health = 0; // Kill the demon
                }
            } 
            else {  // Chase normally
                currentState = CharacterState::RUN;
                faceRight = (playerPos.x > position.x);
            }

            // Allow hits even while exploding, but not after exploded
            if (!exploded) Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);   
        }

        void Draw() override {
            // Backup the real shadow
            Texture2D backupShadow = this->shadow;

            // If Dashing or exploded replace shadow with an empty texture
            if (currentState == CharacterState::DASH || exploded) {
                this->shadow = { 0 }; 
            }
            Mob::Draw();

            // Restore the shadow so it's ready for the next frame
            this->shadow = backupShadow;
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 3.0f : 12.0f;
            return { position.x - (offX * size), position.y - (11.0f * size), 16 * size, 21 * size };
        }

        bool IsDeadAndGone() override {
            if (currentState == CharacterState::DEATH) {
                return animations[CharacterState::DEATH].IsFinished();
            }
            if (currentState == CharacterState::ATTACK_SIDE) {  // Include explosion as dead/gone
                return animations[CharacterState::ATTACK_SIDE].IsFinished();
            }
            return false;
        }

        void OnDeathEffect(Vector2 playerPos) override {
            Blood::SpawnBlood(this->position, playerPos, this->size);
        }
};