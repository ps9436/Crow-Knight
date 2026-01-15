#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Mob.hpp"

class Slime : public Mob {
    private:
        Animation slimeWAKE;
        Animation slimeATTACK;
        Animation slimeTURN;
        Animation slimeHURT;
        Animation slimeDEATH;

        float summonTimer = 0.0f;
        float summonCooldown = 2.0f; // Summons slimes every 2 seconds

    public:
        // For the Game class to read
        bool requestSummon = false;
        float damageInstant = 10.0f;

        static inline Texture2D wakeSlime;
        static inline Texture2D attackSlime;
        static inline Texture2D turnSlime;
        static inline Texture2D hurtSlime;
        static inline Texture2D deadSlime;
        static inline Texture2D shadowSlime;

        Slime () { this->shadow = shadowSlime; }

        static void StaticLoad() {
            Slime::attackSlime = LoadTexture("assets/slime/slime-attack-Sheet.png");
            Slime::hurtSlime = LoadTexture("assets/slime/slime-hurt-Sheet.png");
            Slime::turnSlime = LoadTexture("assets/slime/slime-turn-Sheet.png");
            Slime::deadSlime = LoadTexture("assets/slime/slime-death-Sheet.png");
            Slime::wakeSlime = LoadTexture("assets/slime/slime-wake-Sheet.png");
            Slime::shadowSlime = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(turnSlime);
            UnloadTexture(hurtSlime);
            UnloadTexture(deadSlime);
            UnloadTexture(wakeSlime);
            UnloadTexture(attackSlime);
            UnloadTexture(shadowSlime);
        }

        void init(int scale = 4) {
            size = scale;
            speed = 0.0f;
            health = 4;
            weight = 0.3f;
            radius = 25.0f;
            damageRate = 50.0f; 
            lifestealRate = 3.0f;
            spawnAnim = true;
            
            slimeATTACK.Init(attackSlime, 4, 12.0f, scale); 
            animations[CharacterState::ATTACK_SIDE] = slimeATTACK;

            slimeWAKE.Init(wakeSlime, 4, 4.0f, scale, false);
            animations[CharacterState::IDLE] = slimeWAKE;   // Idle is wake 

            slimeTURN.Init(turnSlime, 4, 12.0f, scale, false);
            animations[CharacterState::DASH] = slimeTURN;
            
            slimeHURT.Init(hurtSlime, 4, 12.0f, scale);
            animations[CharacterState::HURT] = slimeHURT;

            slimeDEATH.Init(deadSlime, 8, 12.0f, scale, false);
            animations[CharacterState::DEATH] = slimeDEATH;

            animations[CharacterState::RUN] = slimeATTACK;  // default is attack
            currentState = CharacterState::IDLE;
        }

        void Update(Vector2 playerPos, bool playerAttacking, Rectangle playerHitbox, Vector2 pushForce, float* globalHitStop, float dt) override {
            // Reset flags
            requestSummon = false;

            if (currentState == CharacterState::HURT || currentState == CharacterState::DEATH) {
                Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);
                return; // Do not run rest
            }

            // faceRight = (playerPos.x > position.x);  // No need to face left or right

            // Timers
            summonTimer += dt;

            if (summonTimer > summonCooldown) {
                    requestSummon = true;
                    summonTimer = 0.0f;
                }
            
            // If IDLE (Wake) finishes, Mob::Update will set currentState to RUN
            Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);

            // Switch from RUN to ATTACK (wake is done)
            if (currentState == CharacterState::RUN) {
                currentState = CharacterState::ATTACK_SIDE;
                animations[CharacterState::ATTACK_SIDE].Reset();
            }
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 3.0f : 12.0f;
            return { position.x - (offX * size), position.y - (11.0f * size), 16 * size, 21 * size };
        }
};