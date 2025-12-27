#pragma once
#include <raylib.h>
#include "Character.hpp"
#include "Input.hpp"

enum class CrowForm { UNDEAD, ALIVE };

class Crow : public Character {
    private:
        CrowForm currentForm = CrowForm::UNDEAD;

        // Crow Stats
        float jumpPower = 800.0f;
        float speed = 350.0f;
        int size;   // Size is 4 unless specified otherwise in Init

        // Dash logic
        float dashDuration = 0.15f;
        float dashTimer = 0.0f;
        Vector2 dashStartPos;
        Vector2 dashTargetPos;

        // Textures
        Texture2D idleUNDEAD; Texture2D runUNDEAD; Texture2D jumpUNDEAD;
        Texture2D idleALIVE; Texture2D runALIVE; Texture2D jumpALIVE;
        Texture2D attackSIDE; Texture2D attackUP; Texture2D attackDOWN;
        Texture2D dashTexture;

        // Animations
        Animation undeadIDLE; Animation undeadRUN; Animation undeadJUMP;
        Animation aliveIDLE; Animation aliveRUN; Animation aliveJUMP;
        Animation sideATTACK; Animation upATTACK; Animation downATTACK;
        Animation dashAnim;

        // Maps<state, anim> for forms
        std::map<CharacterState, Animation> undeadAnims;
        std::map<CharacterState, Animation> aliveAnims;

        // Combat logic
        bool CanInterrupt(CharacterState nextState);
        void Special() override;    // Stab();
        // Helper for the ease in/out math
        float EaseInOutQuad(float t);
        void Dash(Input input);

    public:
        void Init(Vector2 startPos, int scale = 4);
        void Unload();
        void Update(Input input);
        void Draw();

        // Where crow gets hit
        Rectangle GetHitbox() override {
            return { position.x, position.y, 50, 80 };
        }

        // Where crow hits
        Rectangle GetAttackBox() override {
            // If not attacking, return nothing
            if (!IsAttacking()) return { 0, 0, 0, 0 };

            // If we're facing right
            if (faceRight) {
                switch (currentState) {
                case CharacterState::ATTACK_UP:
                if (z > 0.0f) {
                    return { position.x - 14*size, position.y - 53*size - z, 34*size, 63*size };
                } else {
                    return { position.x - 14*size, position.y - 53*size, 34*size, 63*size };
                }
                case CharacterState::ATTACK_DOWN:
                if (z > 0.0f) {
                    return { position.x - 7*size, position.y - 6*size - z, 33*size, 64*size };
                } else {
                    return { position.x - 7*size, position.y - 6*size, 33*size, 64*size };
                }
                case CharacterState::ATTACK_SIDE:
                default:
                    if (z > 0.0f) {
                        return { position.x, position.y - 19*size - z, 60*size, 33*size };
                    } else {
                        return { position.x, position.y - 19*size, 60*size, 33*size };
                    }
                }
            }
            else {
                switch (currentState) {
                    case CharacterState::ATTACK_UP:
                    if (z > 0.0f) {
                        return { position.x - 19*size, position.y - 53*size - z, 34*size, 63*size };
                    } else {
                        return { position.x - 19*size, position.y - 53*size, 34*size, 63*size };
                    }
                    case CharacterState::ATTACK_DOWN:
                    if (z > 0.0f) {
                        return { position.x - 26*size, position.y - 6*size - z, 33*size, 64*size };
                    } else {
                        return { position.x - 26*size, position.y - 6*size, 33*size, 64*size };
                    }
                    case CharacterState::ATTACK_SIDE:
                    default:
                        if (z > 0.0f) {
                            return { position.x - 60*size, position.y - 19*size - z, 60*size, 33*size };
                        } else {
                            return { position.x - 60*size, position.y - 19*size, 60*size, 33*size };
                        }
                }
            }
        }
};