#pragma once
#include <raylib.h>
#include "Mob.hpp"
#include "Input.hpp"

class Goblin : public Mob {
    private:
        Animation goblinRUN;
        Animation goblinRevRUN;
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
            // Stats
            size = scale;
            speed = 150.0f;
            health = 2;
            weight = 1.0f;

            goblinRUN.Init(runGoblin, 4, 8.0f, scale);
            animations[CharacterState::RUN] = goblinRUN;
            
            goblinHURT.Init(hurtGoblin, 4, 12.0f, scale);
            animations[CharacterState::HURT] = goblinHURT;

            goblinDEATH.Init(deadGoblin, 8, 8.0f, scale, false);
            animations[CharacterState::DEATH] = goblinDEATH;

            // Default
            animations[CharacterState::IDLE] = goblinRUN;
            currentState = CharacterState::RUN;
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 5.0f : 13.0f;
            float offY = 13.0f;
            return { position.x - (offX * size), position.y - (offY * size), 18 * size, 21 * size };
        }
};