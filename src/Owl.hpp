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
        static Texture2D runOWL;
        static Texture2D revrunOWL;
        static Texture2D hurtOWL;
        static Texture2D deadOWL;
        static Texture2D shadowOWL;

        Owl () {
            this->shadow = shadowOWL;
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