#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Mob.hpp"

class Necromancer : public Mob {
    private:
        Animation necromancerRUN;
        Animation necromancerHURT;
        Animation necromancerDEATH;
        Animation necromancerATTACK;

        float attackRange = 500.0f; // Range to stop and shoot
        float shootTimer = 0.0f;
        float shootCooldown = 2.0f; // Fires every 2 seconds
        
        float summonTimer = 0.0f;
        float summonCooldown = 8.0f; // Summons every 8 seconds

    public:
        // Flags for the Game class to read
        bool requestShoot = false;
        bool requestSummon = false;

        static inline Texture2D runNecromancer;
        static inline Texture2D hurtNecromancer;
        static inline Texture2D deadNecromancer;
        static inline Texture2D attackNecromancer;
        static inline Texture2D shadowNecromancer;

        Necromancer () { this->shadow = shadowNecromancer; }

        static void StaticLoad() {
            Necromancer::runNecromancer = LoadTexture("assets/necromancer/necromancer-run-Sheet.png");
            Necromancer::hurtNecromancer = LoadTexture("assets/necromancer/necromancer-hurt-Sheet.png");
            Necromancer::deadNecromancer = LoadTexture("assets/necromancer/necromancer-death-Sheet.png");
            Necromancer::attackNecromancer = LoadTexture("assets/necromancer/necromancer-attack-Sheet.png");
            Necromancer::shadowNecromancer = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runNecromancer);
            UnloadTexture(hurtNecromancer);
            UnloadTexture(deadNecromancer);
            UnloadTexture(attackNecromancer);
            UnloadTexture(shadowNecromancer);
        }

        void init(int scale = 4) {
            size = scale;
            speed = 180.0f;
            health = 4;
            weight = 1.0f;
            radius = 25.0f;
            damageRate = 20.0f; 
            lifestealRate = 5.0f;

            necromancerRUN.Init(runNecromancer, 4, 8.0f, scale);
            animations[CharacterState::RUN] = necromancerRUN;
            
            necromancerHURT.Init(hurtNecromancer, 4, 12.0f, scale);
            animations[CharacterState::HURT] = necromancerHURT;

            necromancerDEATH.Init(deadNecromancer, 17, 21.0f, scale, false);
            animations[CharacterState::DEATH] = necromancerDEATH;

            necromancerATTACK.Init(attackNecromancer, 4, 12.0f, scale); 
            animations[CharacterState::ATTACK_SIDE] = necromancerATTACK;

            animations[CharacterState::IDLE] = necromancerATTACK;
            currentState = CharacterState::RUN;
        }

        void Update(Vector2 playerPos, bool playerAttacking, Rectangle playerHitbox, Vector2 pushForce, float* globalHitStop, float dt) override {
            // Reset flags
            requestShoot = false;
            requestSummon = false;

            if (currentState == CharacterState::HURT || currentState == CharacterState::DEATH) {
                Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);
                return; // Do not run rest
            }

            // If not hurt or dead
            float distToPlayer = Vector2Distance(position, playerPos);
            faceRight = (playerPos.x > position.x);

            // Timers
            shootTimer += dt;
            summonTimer += dt;

            if (summonTimer > summonCooldown) {
                    requestSummon = true;
                    summonTimer = 0.0f;
                }

            // Determine attack or chase
            if (distToPlayer <= attackRange) {
                currentState = CharacterState::ATTACK_SIDE;
                animations[CharacterState::ATTACK_SIDE].Reset();
            } else {
                currentState = CharacterState::RUN;
            }

            // When attacking
            if (currentState == CharacterState::ATTACK_SIDE) {
                animations[CharacterState::ATTACK_SIDE].Update(dt);
                if (shootTimer > shootCooldown) {
                    requestShoot = true; 
                    shootTimer = 0.0f; 
                }
            }

            Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 3.0f : 12.0f;
            return { position.x - (offX * size), position.y - (11.0f * size), 16 * size, 21 * size };
        }
};