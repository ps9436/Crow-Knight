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
};