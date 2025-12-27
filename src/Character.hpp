#pragma once
#include <raylib.h>
#include "Animation.hpp"
#include <map>

// Generic states shared by all playable characters
enum class CharacterState {
    IDLE,
    RUN,
    JUMP,
    ATTACK_SIDE,
    ATTACK_UP,
    ATTACK_DOWN,
    DASH,       // Or other (for mobs)
    HURT,
    DEATH
};

class Character {
    protected:
        Vector2 position;
        Vector2 velocity;           // X/Y movement
        float z = 0.0f;             // Z-axis height
        float zVelocity = 0.0f;     // Vertical (jump) velocity
        float gravity = 2000.0f;
        bool faceRight = false;     // Should character face right (y/n)?
        bool onGround = true;
        Vector2 dashEffectPos;
        Texture2D shadow;

        std::map<CharacterState, Animation> animations;
        CharacterState currentState = CharacterState::IDLE;

        void DrawShadow();

    public:
        virtual void Update(float dt);
        virtual void Draw();

        // Helper to streamline loading
        void AddAnimation(CharacterState state, Animation anim);
        // Helper to check if current state is in any attacks
        bool IsAttacking() const {
            return currentState == CharacterState::ATTACK_SIDE || 
                   currentState == CharacterState::ATTACK_UP || 
                   currentState == CharacterState::ATTACK_DOWN;
        }

        virtual void Special() {}   // {} so we don't have to implement

        // Virtual so specific characters can override it for special logic
        virtual void OnStateSwitch(CharacterState oldState, CharacterState newState) {}

        virtual Rectangle GetHitbox() {}        // Hurtbox (where the character gets hurt)
        virtual Rectangle GetAttackBox() {}     // Where character hits

        // Helpers
        // Check if we are state where we can't move
        bool IsLocked() const {
            return currentState == CharacterState::DEATH;
        }
        
        // Get character center
        Vector2 GetPosition() {
            return { position.x, position.y}; 
        }

        // Get character facing (R = true/L = flase)
        bool GetFaceRight() {
            return { faceRight };
        }
};