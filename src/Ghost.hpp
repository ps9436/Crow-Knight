#pragma once
#include <raylib.h>
#include "Mob.hpp"
#include "Input.hpp"

class Ghost : public Mob {
    private:
        Animation ghostRUN;
        Animation ghostHURT;
        Animation ghostDEATH;
        Animation ghostWake;

    public:
        static inline Texture2D runGhost;
        static inline Texture2D hurtGhost;
        static inline Texture2D deadGhost;
        static inline Texture2D wakeGhost;

        // Load ghost textures
        static void StaticLoad() {
            Ghost::runGhost = LoadTexture("assets/ghost/ghost-run-Sheet.png");
            Ghost::hurtGhost = LoadTexture("assets/ghost/ghost-hurt-Sheet.png");
            Ghost::deadGhost = LoadTexture("assets/ghost/ghost-death-Sheet.png");
            Ghost::wakeGhost = LoadTexture("assets/ghost/ghost-wake-Sheet.png");
        }

        static void StaticUnload() {
            UnloadTexture(runGhost);
            UnloadTexture(hurtGhost);
            UnloadTexture(deadGhost);
            UnloadTexture(wakeGhost);
        }

        void init(int scale = 4) {
            // Stats (unique to ghost)
            size = scale;
            speed = 200.0f;
            health = 2;
            weight = 0.0f;
            radius = 0.0f;
            damageRate = 30.0f;
            lifestealRate = 0.1f;
            stunAmount = 24;
            spawnAnim = true;

            ghostRUN.Init(runGhost, 4, 8.0f, scale);
            animations[CharacterState::RUN] = ghostRUN;
            ghostHURT.Init(hurtGhost, 4, 6.0f, scale);
            animations[CharacterState::HURT] = ghostHURT;
            ghostDEATH.Init(deadGhost, 9, 24.0f, scale, false);
            animations[CharacterState::DEATH] = ghostDEATH;
            ghostWake.Init(wakeGhost, 4, 4.0f, scale, false);
            animations[CharacterState::IDLE] = ghostWake;

            currentState = CharacterState::IDLE;
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 3.0f : 12.0f;
            float offY = 11.0f;
            return { position.x - (offX * size), position.y - (offY * size), 16 * size, 21 * size };
        }

        int GetXPValue() override {
            return 7;
        }
};