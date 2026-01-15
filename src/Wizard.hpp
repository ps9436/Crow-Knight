#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Mob.hpp"

class Wizard : public Mob {
    private:
        Animation wizardRUN;
        Animation wizardHURT;
        Animation wizardDEATH;
        Animation wizardATTACK;

        float attackRange = 500.0f; // Range to stop and shoot
        float shootTimer = 0.0f;
        float shootCooldown = 2.0f; // Fires every 2 seconds

    public:
        // Flag for the Game class to read
        bool requestShoot = false;

        static inline Texture2D runWizard;
        static inline Texture2D hurtWizard;
        static inline Texture2D deadWizard;
        static inline Texture2D attackWizard;
        static inline Texture2D shadowWizard;

        Wizard () { this->shadow = shadowWizard; }

        static void StaticLoad() {
            Wizard::runWizard = LoadTexture("assets/wizard/wizard-run-Sheet.png");
            Wizard::hurtWizard = LoadTexture("assets/wizard/wizard-hurt-Sheet.png");
            Wizard::deadWizard = LoadTexture("assets/wizard/wizard-death-Sheet.png");
            Wizard::attackWizard = LoadTexture("assets/wizard/wizard-attack-Sheet.png");
            Wizard::shadowWizard = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runWizard);
            UnloadTexture(hurtWizard);
            UnloadTexture(deadWizard);
            UnloadTexture(attackWizard);
            UnloadTexture(shadowWizard);
        }

        void init(int scale = 4) {
            size = scale;
            speed = 180.0f;
            health = 4;
            weight = 1.0f;
            radius = 25.0f;
            damageRate = 1.0f; 
            lifestealRate = 5.0f;

            wizardRUN.Init(runWizard, 4, 8.0f, scale);
            animations[CharacterState::RUN] = wizardRUN;
            
            wizardHURT.Init(hurtWizard, 4, 12.0f, scale);
            animations[CharacterState::HURT] = wizardHURT;

            wizardDEATH.Init(deadWizard, 17, 21.0f, scale, false);
            animations[CharacterState::DEATH] = wizardDEATH;

            wizardATTACK.Init(attackWizard, 4, 12.0f, scale); 
            animations[CharacterState::ATTACK_SIDE] = wizardATTACK;

            animations[CharacterState::IDLE] = wizardATTACK;
            currentState = CharacterState::RUN;
        }

        void Update(Vector2 playerPos, bool playerAttacking, Rectangle playerHitbox, Vector2 pushForce, float* globalHitStop, float dt) override {
            // Reset flags
            requestShoot = false;

            if (currentState == CharacterState::HURT || currentState == CharacterState::DEATH) {
                Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);
                return; // Do not run rest
            }

            // If not hurt or dead
            float distToPlayer = Vector2Distance(position, playerPos);
            faceRight = (playerPos.x > position.x);

            // Timers
            shootTimer += dt;
            
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