#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Mob.hpp"

class Slimeball : public Mob {
    private:
        Animation slimeballRUN;
        Animation slimeballHURT;
        Animation slimeballDEATH;
        Animation slimeballWAKE;
        Animation slimeballJUMP;

        float jumpRange = 300.0f; // Jump distance
        float jumpPower = 600.0f;

    public:
        static inline Texture2D runSlimeball;
        static inline Texture2D hurtSlimeball;
        static inline Texture2D deadSlimeball;
        static inline Texture2D wakeSlimeball;
        static inline Texture2D jumpSlimeball;
        static inline Texture2D shadowSlimeball;

        Slimeball () { this->shadow = shadowSlimeball; }

        static void StaticLoad() {
            Slimeball::runSlimeball = LoadTexture("assets/slimeball/slimeball-run-Sheet.png");
            Slimeball::hurtSlimeball = LoadTexture("assets/slimeball/slimeball-hurt-Sheet.png");
            Slimeball::deadSlimeball = LoadTexture("assets/slimeball/slimeball-death-Sheet.png");
            Slimeball::wakeSlimeball = LoadTexture("assets/slimeball/slimeball-wake-Sheet.png");
            Slimeball::jumpSlimeball = LoadTexture("assets/slimeball/slimeball-jump-Sheet.png");
            Slimeball::shadowSlimeball = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runSlimeball);
            UnloadTexture(hurtSlimeball);
            UnloadTexture(deadSlimeball);
            UnloadTexture(wakeSlimeball);
            UnloadTexture(jumpSlimeball);
            UnloadTexture(shadowSlimeball);
        }

        void init(int scale = 4) {
            size = scale;
            speed = 300.0f;
            health = 1;     // if > 1 then they will jump again after being hurt, missing the player since closer
            weight = 1.0f;
            radius = 15.0f;
            damageRate = 10.0f;
            lifestealRate = 0.2f;
            spawnAnim = true;

            slimeballRUN.Init(runSlimeball, 4, 12.0f, scale);
            animations[CharacterState::RUN] = slimeballRUN;
            
            slimeballHURT.Init(hurtSlimeball, 4, 12.0f, scale);
            animations[CharacterState::HURT] = slimeballHURT;

            slimeballDEATH.Init(deadSlimeball, 8, 12.0f, scale, false);
            animations[CharacterState::DEATH] = slimeballDEATH;

            slimeballWAKE.Init(wakeSlimeball, 4, 4.0f, scale, false); 
            animations[CharacterState::ATTACK_SIDE] = slimeballWAKE;

            slimeballJUMP.Init(jumpSlimeball, 4, 4.0f, scale, false); 
            animations[CharacterState::JUMP] = slimeballJUMP;

            animations[CharacterState::IDLE] = slimeballWAKE;
            currentState = CharacterState::RUN;
        }

        void Update(Vector2 playerPos, bool playerAttacking, Rectangle playerHitbox, Vector2 pushForce, float* globalHitStop, float dt) override {

            if (currentState == CharacterState::HURT || currentState == CharacterState::DEATH) {
                Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);
                return; // Do not run rest
            }

            float distToPlayer = Vector2Distance(position, playerPos);

            // 2. Trigger Jump logic ONLY if we are currently running
            if (currentState == CharacterState::RUN) {
                faceRight = (playerPos.x > position.x);

                if (distToPlayer <= jumpRange) {
                    currentState = CharacterState::JUMP;
                    
                    // Start the jump animation
                    if (animations.count(CharacterState::JUMP)) {
                        animations[CharacterState::JUMP].Reset();
                    }

                    // Set Vertical Impulse
                    zVelocity = jumpPower;

                    // Calculate Horizontal Velocity (Leap toward player)
                    Vector2 dir = Vector2Normalize(Vector2Subtract(playerPos, position));
                    velocity = Vector2Scale(dir, speed*1.5); 
                }
            }

            // 3. Handle Landing
            if (currentState == CharacterState::JUMP && onGround && zVelocity <= 0) {
                currentState = CharacterState::DEATH;
                velocity = {0, 0}; // Stop sliding after landing
            }

            Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);
        }

        Rectangle GetHitbox() override {
            // add an if statement if slimeball can't hit when in the air
            float offX = faceRight ? 3.0f : 12.0f;
            return { position.x - (offX * size), position.y - (11.0f * size) - z, 16 * size, 21 * size };
        }
};