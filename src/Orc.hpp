#pragma once
#include <raylib.h>
#include "Mob.hpp"
#include "Input.hpp"

class Orc : public Mob {
    private:
        Animation orcRUN;
        Animation orcHURT;
        Animation orcDEATH;

    public:
        static inline Texture2D runOrc;
        static inline Texture2D hurtOrc;
        static inline Texture2D deadOrc;
        static inline Texture2D shadowOrc;

        Orc () {
            this->shadow = shadowOrc;
        }

        // Load orc textures
        static void StaticLoad() {
            Orc::runOrc = LoadTexture("assets/orc/orc-run-Sheet.png");
            Orc::hurtOrc = LoadTexture("assets/orc/orc-hurt-Sheet.png");
            Orc::deadOrc = LoadTexture("assets/orc/orc-death-Sheet.png");
            Orc::shadowOrc = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runOrc);
            UnloadTexture(hurtOrc);
            UnloadTexture(deadOrc);
            UnloadTexture(shadowOrc);
        }

        void init(int scale = 4) {
            // Stats (unique to orc)
            size = scale;
            speed = 250.0f;
            health = 3;
            weight = 0.75f;
            radius = 20.0f;

            orcRUN.Init(runOrc, 4, 8.0f, scale);
            animations[CharacterState::RUN] = orcRUN;
            
            orcHURT.Init(hurtOrc, 4, 12.0f, scale);
            animations[CharacterState::HURT] = orcHURT;

            orcDEATH.Init(deadOrc, 8, 8.0f, scale, false);
            animations[CharacterState::DEATH] = orcDEATH;

            // Default
            animations[CharacterState::IDLE] = orcRUN;
            currentState = CharacterState::RUN;
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 3.0f : 12.0f;
            float offY = 11.0f;
            return { position.x - (offX * size), position.y - (offY * size), 16 * size, 21 * size };
        }
};