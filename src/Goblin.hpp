#pragma once
#include <raylib.h>
#include "Mob.hpp"
#include "Input.hpp"

class Goblin : public Mob {
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

        Goblin () {
            this->shadow = shadowOWL;
        }

        // Load owl textures
        static void StaticLoad() {
            Goblin::runOWL = LoadTexture("assets/goblin/goblin-run.png");
            Goblin::hurtOWL = LoadTexture("assets/goblin/goblin-hurt.png");
            Goblin::deadOWL = LoadTexture("assets/goblin/goblin-death.png");
            Goblin::shadowOWL = LoadTexture("assets/Shadow.png");
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