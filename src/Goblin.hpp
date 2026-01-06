#pragma once
#include <raylib.h>
#include "Mob.hpp"
#include "Input.hpp"

class Goblin : public Mob {
    private:
        Animation goblinRUN;
        Animation goblinHURT;
        Animation goblinDEATH;

    public:
        static inline Texture2D runGoblin;
        static inline Texture2D hurtGoblin;
        static inline Texture2D deadGoblin;
        static inline Texture2D shadowGoblin;

        Goblin () {
            this->shadow = shadowGoblin;
        }

        // Load goblin textures
        static void StaticLoad() {
            Goblin::runGoblin = LoadTexture("assets/goblin/goblin-run-Sheet.png");
            Goblin::hurtGoblin = LoadTexture("assets/goblin/goblin-hurt-Sheet.png");
            Goblin::deadGoblin = LoadTexture("assets/goblin/goblin-death-Sheet.png");
            Goblin::shadowGoblin = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runGoblin);
            UnloadTexture(hurtGoblin);
            UnloadTexture(deadGoblin);
            UnloadTexture(shadowGoblin);
        }

        void init(int scale = 4) {
            // Stats (unique to goblin)
            size = scale;
            speed = 200.0f;
            health = 2;
            weight = 1.0f;
            radius = 20.0f;
            damageRate = 20.0f;
            lifestealRate = 1.0f;

            goblinRUN.Init(runGoblin, 4, 8.0f, scale);
            animations[CharacterState::RUN] = goblinRUN;
            
            goblinHURT.Init(hurtGoblin, 4, 12.0f, scale);
            animations[CharacterState::HURT] = goblinHURT;

            goblinDEATH.Init(deadGoblin, 8, 12.0f, scale, false);
            animations[CharacterState::DEATH] = goblinDEATH;

            // Default
            animations[CharacterState::IDLE] = goblinRUN;
            currentState = CharacterState::RUN;
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 3.0f : 12.0f;
            float offY = 11.0f;
            return { position.x - (offX * size), position.y - (offY * size), 16 * size, 21 * size };
        }

        int GetXPValue() override {
            return 1;
        }

        void OnDeathEffect(Vector2 playerPos) override {
            Blood::SpawnBlood(this->position, playerPos, this->size);
        }
};