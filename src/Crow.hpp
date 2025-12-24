#pragma once
#include <raylib.h>
#include "Animation.hpp"
#include "Input.hpp"

enum class CrowForm {
    Undead,
    Alive
};

enum class CrowState {
    Idle,
    Run,
    Jump,
    Attack
};

enum class AttackType {
    Side,
    Up,
    Down
};

class Crow {
    public:
        // Actual main.cpp setup
        void Init(Vector2 startPos, int spriteScale);
        void Update(Input input);
        void Draw();
        void Unload();

        // Helpers
        void SetPosition(Vector2 pos);
        Vector2 GetPosition() const { return position; }
        Animation& GetAnimation();
        Animation& GetAttackAnimation();
        void StartAttack(Input input);
    private:
        Vector2 position{};

        float gravity = 2000.0f;
        float moveSpeed = 350.0f;
        float z = 0.0f;                // Z-axis floor
        float jumpPower = 800.0f;
        float zVelocity = 0.0f;        // Vertical (jump) velocity
        bool onGround = true;
        bool faceRight = false;
        
        bool isAttacking = false;
        AttackType attack { AttackType::Side };
        struct AttackTimeline {
            AttackType type;
            int activeStartFrame;
            int activeEndFrame;

            bool hitboxOn = false;
            bool finished = false;
            bool dashCanceled = false;
        };

        // int scale{ 1 };
        CrowForm form { CrowForm::Undead };
        void Stab();    // Switch from Undead/Alive

        CrowState state { CrowState::Idle };
        void DetermineState(Input input);
        void SetState(CrowState newState);

        Animation aliveIdle;
        Animation aliveRun;
        Animation aliveJump;
        Animation sideAttack;
        Animation upAttack;
        Animation downAttack;
        Animation undeadIdle;
        Animation undeadRun;
        Animation undeadJump;
};