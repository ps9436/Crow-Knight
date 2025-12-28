#pragma once
#include <raylib.h>
#include "Mob.hpp"
#include "Input.hpp"

class Owl : public Mob {
    private:
        Animation owlRUN;
        Animation owlRevRUN;
        Animation owlHURT;
        Animation owlDEATH;

    public:
        static inline Texture2D runOWL;
        static inline Texture2D hurtOWL;
        static inline Texture2D deadOWL;
        static inline Texture2D shadowOWL;

        Owl () {
            this->shadow = shadowOWL;
        }

        // Load owl textures
        static void StaticLoad() {
            Owl::runOWL = LoadTexture("assets/goblin-run.png");
            Owl::hurtOWL = LoadTexture("assets/goblin-hurt.png");
            Owl::deadOWL = LoadTexture("assets/goblin-death2.png");
            Owl::shadowOWL = LoadTexture("assets/Charles-Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runOWL);
            UnloadTexture(hurtOWL);
            UnloadTexture(deadOWL);
            UnloadTexture(shadowOWL);
        }

        void init(int scale = 4) {
            // Stats
            size = scale;
            speed = 250.0f;
            health = 2;
            weight = 1.0f;

            owlRUN.Init(runOWL, 4, 12.0f, scale);
            animations[CharacterState::RUN] = owlRUN;

            // owlRevRUN.Init(revrunOWL, 4, 16.0f, scale);
            // animations[CharacterState::RUN] = owlRevRUN;

            owlHURT.Init(hurtOWL, 4, 16.0f, scale);
            animations[CharacterState::HURT] = owlHURT;

            owlDEATH.Init(deadOWL, 8, 8.0f, scale, false);
            animations[CharacterState::DEATH] = owlDEATH;

            // Default
            animations[CharacterState::IDLE] = owlRUN;
            currentState = CharacterState::RUN;
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 5.0f : 13.0f;
            float offY = 13.0f;
            return { position.x - (offX * size), position.y - (offY * size), 18 * size, 21 * size };
        }
};