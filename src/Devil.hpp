#pragma once
#include <raylib.h>
#include <raymath.h>
#include "Mob.hpp"

class Devil : public Mob {
    private:
        Animation devilRUN;
        Animation devilHURT;
        Animation devilDEATH;
        Animation devilATTACK;
        Animation devilDASH;

        bool isExploding = false;

    public:
        // Flag for Game
        bool exploded = false;
        bool dealtDamage = false;
        float explosionRad = 180.0f;
        float damageInstant = 5000.0f;

        static inline Texture2D runDevil;
        static inline Texture2D hurtDevil;
        static inline Texture2D deadDevil;
        static inline Texture2D attackDevil;
        static inline Texture2D shadowDevil;

        Devil () { this->shadow = shadowDevil; }

        static void StaticLoad() {
            Devil::runDevil = LoadTexture("assets/devil/devil-run-Sheet.png");
            Devil::hurtDevil = LoadTexture("assets/devil/devil-hurt-Sheet.png");
            Devil::deadDevil = LoadTexture("assets/devil/devil-death-Sheet.png");
            Devil::attackDevil = LoadTexture("assets/devil/devil-attack-Sheet.png");
            Devil::shadowDevil = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runDevil);
            UnloadTexture(hurtDevil);
            UnloadTexture(deadDevil);
            UnloadTexture(attackDevil);
            UnloadTexture(shadowDevil);
        }

        void init(int scale = 6) {
            size = scale;
            speed = 150.0f;
            health = 8;
            weight = 0.3f;
            radius = 30.0f;
            damageRate = 50.0f; 
            lifestealRate = 5.0f;
            stunAmount = 10;

            devilRUN.Init(runDevil, 4, 8.0f, scale);
            animations[CharacterState::RUN] = devilRUN;
            
            devilHURT.Init(hurtDevil, 4, 12.0f, scale);
            animations[CharacterState::HURT] = devilHURT;

            devilDEATH.Init(deadDevil, 20, 12.0f, scale, false);
            animations[CharacterState::DEATH] = devilDEATH;

            devilATTACK.Init(attackDevil, 20, 12.0f, scale, false); 
            animations[CharacterState::ATTACK_SIDE] = devilATTACK;

            animations[CharacterState::IDLE] = devilRUN;
            currentState = CharacterState::RUN;
        }

        void Update(Vector2 playerPos, bool playerAttacking, Rectangle playerHitbox, Vector2 pushForce, float* globalHitStop, float dt) override {

            if (currentState == CharacterState::HURT || currentState == CharacterState::DEATH) {
                Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);
                return; // Do not run rest
            }

            // Start exploding when HP is 4
            if (health <= 4 && !isExploding) {
                isExploding = true;
    
                // Enter Attack
                currentState = CharacterState::ATTACK_SIDE;
                animations[CharacterState::ATTACK_SIDE].Reset();
            }
            
            if (currentState == CharacterState::ATTACK_SIDE) {
                // Exploding
                if (exploded) animations[CharacterState::ATTACK_SIDE].Update(dt);
                if (isExploding) animations[CharacterState::ATTACK_SIDE].Update(dt);

                // Explosion after frame 11
                if (animations[CharacterState::ATTACK_SIDE].currentFrame > 11) {
                    exploded = true;
                    health = 0; // Kill the devil
                }
            } 
            else {  // Chase normally
                currentState = CharacterState::RUN;
                faceRight = (playerPos.x > position.x);
            }

            // Allow hits even while exploding, but not after exploded
            if (!isExploding) Mob::Update(playerPos, playerAttacking, playerHitbox, pushForce, globalHitStop, dt);   
        }

        void Draw() override {
            // Backup real shadow
            Texture2D backupShadow = this->shadow;

            // If exploded replace shadow with an empty texture
            if (exploded) this->shadow = { 0 };
            Mob::Draw();

            // Restore shadow
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