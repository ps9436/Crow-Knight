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

            owlRUN.Init(runOWL, 4, 16.0f, scale);
            animations[CharacterState::RUN] = owlRUN;

            // owlRevRUN.Init(revrunOWL, 4, 16.0f, scale);
            // animations[CharacterState::RUN] = owlRevRUN;

            owlHURT.Init(hurtOWL, 4, 16.0f, scale);
            animations[CharacterState::HURT] = owlHURT;

            owlDEATH.Init(deadOWL, 10, 10.0f, scale, false);
            animations[CharacterState::DEATH] = owlDEATH;

            // Default
            animations[CharacterState::IDLE] = owlRUN;
            currentState = CharacterState::RUN;

            // Stats
            speed = 300.0f;
            health = 2;
        }

        Rectangle GetHitbox() override {
            if (faceRight) {
                return { position.x - 20, position.y - 52, 72, 84 };
            } else {
                return { position.x - 52, position.y - 52, 72, 84 };
            }
        }
};