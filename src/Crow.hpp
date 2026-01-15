#pragma once
#include <raylib.h>
#include "Character.hpp"
#include "Input.hpp"

enum class CrowForm { UNDEAD, ALIVE };

class Crow : public Character {
    private:
        friend class Upgrade;

        CrowForm currentForm = CrowForm::UNDEAD;

        // Crow Stats
        int level = 1;
        float currentXP = 0;
        float nextLevelXP = 10;
        int attackSpeed = 0;       // Lower is faster 3 -> 0
        float jumpPower = 500.0f;
        float speed = 300.0f;
        float BLOOD = 100.0f;
        float currentBlood = 100.0f;
        float bloodDrainRate = 0.0f;
        float lifeSteal = 0.5f;     // Rate/amount of lifesteal
        float lifeStolen = 0.0f;
        
        int killCount;
        bool leveledUp;
        float bloodDrainMultiplier = 1.0f;
        float rotTimer = 0.0f;

        bool isHurt;    // For blood animation
        bool switched;  // For switch blood animation

        // Dash logic
        float dashDuration = 0.15f; // How fast dash is executed
        float dashTimer = 0.0f;
        float dashCost = 0.0f;
        Vector2 dashStartPos;
        Vector2 dashTargetPos;
        Vector2 dashEffectPos;

        // Textures
        Texture2D idleUNDEAD; Texture2D runUNDEAD; Texture2D jumpUNDEAD;
        Texture2D idleALIVE; Texture2D runALIVE; Texture2D jumpALIVE;
        Texture2D attackSIDE; Texture2D attackUP; Texture2D attackDOWN;
        Texture2D dashTexture; Texture2D hurtTexture; Texture2D switchTexture;

        // Animations
        Animation undeadIDLE; Animation undeadRUN; Animation undeadJUMP;
        Animation aliveIDLE; Animation aliveRUN; Animation aliveJUMP;
        Animation sideATTACK; Animation upATTACK; Animation downATTACK;
        Animation dashAnim; Animation hurtAnim; Animation switchAnim;

        // Maps<state, anim> for forms
        std::map<CharacterState, Animation> undeadAnims;
        std::map<CharacterState, Animation> aliveAnims;

        // Combat logic
        bool CanInterrupt(CharacterState nextState);
        void Dash(Input input, float dt);
        float EaseInOutQuad(float t);   // Helper for the ease in/out math
        void Special() override;        // Stab();

    public:
        void Init(Vector2 startPos, int scale = 4);
        void Unload();
        void Update(Input input, float dt);
        void Draw();
        // Combat logic
        void LifeSteal(float amount);
        void Heal();
        void TakeDamageDt(float amount);
        void TakeInstantDamage(float amount);
        float GetBloodPercent();
        float GetLifeStealPercent();
        bool IsAlive() const;

        void Reset(Vector2 startPos) {
            // Stats reset
            level = 1;
            currentXP = 0;
            nextLevelXP = 30;
            leveledUp = false;
            attackSpeed = 0;
            jumpPower = 800.0f;
            speed = 350.0f;
            BLOOD = 100.0f;
            currentBlood = 100.0f;
            bloodDrainRate = 0.0f;
            lifeSteal = 0.5f;
            lifeStolen = 0.0f;
            killCount = 0;

            // Dash reset
            dashDuration = 0.15f;
            dashTimer = 0.0f;
            dashCost = 0.0f;

            // Form reset
            currentForm = CrowForm::UNDEAD;
            animations = undeadAnims;
            currentState = CharacterState::IDLE;
            if (animations.count(currentState) > 0) animations[currentState].Reset();
            faceRight = false;
            
            // Physics reset
            velocity = { 0, 0 };
            z = 0.0f;
            zVelocity = 0.0f;
        }

        // Where crow gets hit
        Rectangle GetHitbox() override {
            if (z > 0.0f) return {0,0,0,0}; // Invincible while jumping
            float offX = faceRight ? 0.0f : 10.0f;
            float offY = 9.0f;
            float zOffset = (z > 0.0f) ? z : 0.0f;
            return { position.x - (offX * size), position.y - (offY * size) - zOffset, 10 * size, 16 * size };
        }

        // Where crow hits
        Rectangle GetAttackBox() override {
            // If not attacking, return nothing
            if (!IsAttacking()) return { 0, 0, 0, 0 };
            // Active frames are frame 1
            if (GetCurrentFrame() < 1 || GetCurrentFrame() > 2) return { 0, 0, 0, 0};

            // Width, Height, Offset center
            float w, h, offY, offX;
            
            // Determine box shape based on State
            switch (currentState) {
                case CharacterState::ATTACK_UP:
                    w = 34; h = 63; offY = 53;
                    // If right, shift left by 14, else shift left by 19
                    offX = faceRight ? -14 : -19; 
                    break;

                case CharacterState::ATTACK_DOWN:
                    w = 33; h = 64; offY = 6;
                    offX = faceRight ? -7 : -26;
                    break;

                case CharacterState::ATTACK_SIDE:
                default:
                    w = 60; h = 33; offY = 19;
                    offX = faceRight ? 0 : -60; 
                    break;
            }

            // Apply scale (size of character) and Z-height
            float zOffset = (z > 0.0f) ? z : 0.0f;

            return {
                position.x + (offX * size),
                position.y - (offY * size) - zOffset,
                w * size,
                h * size
            };
        }

        void ApplyBloodRot() {
            bloodDrainMultiplier = 1.0f; // Double drain speed
            rotTimer = 5.0f;             // Lasts 5 seconds
        }

        // PUT THESE IN CHARACTER.HPP !!!!!!!!111111111111
        void KillPlusOne() { killCount++; }

        void GainXP(float amount) {
            currentXP += amount;
            while (currentXP >= nextLevelXP) {
                LevelUp();
            }
        }

        void LevelUp() {
            level++;
            currentXP -= nextLevelXP; // Keep overflow XP
            nextLevelXP *= 2.0f; // Harder to get next level
            
            leveledUp = true;
        }

        // Getters for Game
        bool LeveledUp() { return leveledUp; }

        void ClearLevelUpFlag() { leveledUp = false; }

        // Getters for HUD
        int GetKillCount() { return killCount; }

        int GetLevel() { return level; }
        
        float GetXPPercent() { return currentXP / nextLevelXP; }
};