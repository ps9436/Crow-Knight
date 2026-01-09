#pragma once
#include <raylib.h>
#include "Mob.hpp"
#include "Input.hpp"

class Golem : public Mob {
    private:
        Animation golemRUN;
        Animation golemHURT;
        Animation golemDEATH;

    public:
        static inline Texture2D runGolem;
        static inline Texture2D hurtGolem;
        static inline Texture2D deadGolem;
        static inline Texture2D shadowGolem;

        Golem () {
            this->shadow = shadowGolem;
        }

        // Load golem textures
        static void StaticLoad() {
            Golem::runGolem = LoadTexture("assets/golem/golem-run-Sheet.png");
            Golem::hurtGolem = LoadTexture("assets/golem/golem-hurt-Sheet.png");
            Golem::deadGolem = LoadTexture("assets/golem/golem-death-Sheet.png");
            Golem::shadowGolem = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runGolem);
            UnloadTexture(hurtGolem);
            UnloadTexture(deadGolem);
            UnloadTexture(shadowGolem);
        }

        void init(int scale = 6) {
            // Stats (unique to golem)
            size = scale;
            speed = 125.0f;
            health = 10;
            weight = 0.25f;
            radius = 30.0f;
            damageRate = 50.0f;
            lifestealRate = 1.0f;
            stunAmount = 10;

            golemRUN.Init(runGolem, 4, 6.0f, scale);
            animations[CharacterState::RUN] = golemRUN;
            golemHURT.Init(hurtGolem, 4, 12.0f, scale);
            animations[CharacterState::HURT] = golemHURT;
            golemDEATH.Init(deadGolem, 22, 8.0f, scale, false);
            animations[CharacterState::DEATH] = golemDEATH;
            // Default
            animations[CharacterState::IDLE] = golemRUN;
            currentState = CharacterState::RUN;
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 3.0f : 12.0f;
            float offY = 11.0f;
            return { position.x - (offX * size), position.y - (offY * size), 16 * size, 21 * size };
        }

        int GetXPValue() override {
            return 20;
        }

        void OnDeathEffect(Vector2 playerPos) override {
            Blood::SpawnBlood(this->position, playerPos, this->size);
        }
};