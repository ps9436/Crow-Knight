#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Animation.hpp"

struct BloodTemplate {
    Texture2D texture;
    int frameCount;
    float frameSpeed;
};

struct BloodSplatter {
    Vector2 position;
    Animation anim;
    bool active;
    bool flipH;
};

class Blood {
    public:
        static inline std::vector<BloodTemplate> upTemplates;
        static inline std::vector<BloodTemplate> downTemplates;
        static inline std::vector<BloodTemplate> sideTemplates;

        static inline std::vector<BloodSplatter> splatters;

        static void LoadTextures() {
            upTemplates.push_back({ LoadTexture("assets/blood/up1-blood.png"), 8, 24.0f });
            upTemplates.push_back({ LoadTexture("assets/blood/up2-blood.png"), 10, 30.0f });
            upTemplates.push_back({ LoadTexture("assets/blood/up3-blood.png"), 9, 27.0f });
            upTemplates.push_back({ LoadTexture("assets/blood/up4-blood.png"), 9, 27.0f });
            upTemplates.push_back({ LoadTexture("assets/blood/up5-blood.png"), 7, 21.0f });
            downTemplates.push_back({ LoadTexture("assets/blood/down1-blood.png"), 8, 24.0f });
            downTemplates.push_back({ LoadTexture("assets/blood/down2-blood.png"), 10, 30.0f });
            downTemplates.push_back({ LoadTexture("assets/blood/down3-blood.png"), 9, 27.0f });
            downTemplates.push_back({ LoadTexture("assets/blood/down4-blood.png"), 7, 21.0f });
            sideTemplates.push_back({ LoadTexture("assets/blood/side1-blood.png"), 9, 27.0f });
            sideTemplates.push_back({ LoadTexture("assets/blood/side2-blood.png"), 8, 24.0f });
            sideTemplates.push_back({ LoadTexture("assets/blood/side3-blood.png"), 9, 27.0f });
            sideTemplates.push_back({ LoadTexture("assets/blood/side4-blood.png"), 7, 21.0f });
        }

        static void UnloadTextures() {
            for (BloodTemplate tem : upTemplates) UnloadTexture(tem.texture);
            for (BloodTemplate tem : downTemplates) UnloadTexture(tem.texture);
            for (BloodTemplate tem : sideTemplates) UnloadTexture(tem.texture);
        }

        static void SpawnBlood(Vector2 mobPos, Vector2 playerPos, int scale = 4) {
            // Create instance
            BloodSplatter newSplatter;
            newSplatter.position = mobPos;
            newSplatter.active = true;
            newSplatter.flipH = (playerPos.x > mobPos.x);

            Vector2 dir = Vector2Subtract(mobPos, playerPos);
            float angle = atan2(dir.y, dir.x) * (180.0f / PI);  // Angle of mob relative to player

            BloodTemplate selectedTem;

            // Check direction of mob and select texture
            if (angle > -45 && angle <= 45) {
                // Right side
                selectedTem = sideTemplates[GetRandomValue(0, sideTemplates.size() - 1)];
            } else if (angle > 45 && angle <= 135) {
                // Down
                selectedTem = downTemplates[GetRandomValue(0, downTemplates.size() - 1)];
            } else if (angle > 135 || angle <= -135) {
                // Left
                selectedTem = sideTemplates[GetRandomValue(0, sideTemplates.size() - 1)];
            } else {
                // Up
                selectedTem = upTemplates[GetRandomValue(0, upTemplates.size() - 1)];
            }

            // Initialize animation with static texture (not object-unique tex)
            newSplatter.anim.Init(selectedTem.texture, selectedTem.frameCount, selectedTem.frameSpeed, scale, false);
            splatters.push_back(newSplatter);
        }

        static void Update(float dt) {
            for (auto& s : splatters) {
                s.anim.Update(dt);
                if (s.anim.IsFinished()) s.active = false;
            }
            splatters.erase(std::remove_if(splatters.begin(), splatters.end(), 
                [](const BloodSplatter& s) { return !s.active; }), splatters.end());   
        }
        
        static void Draw() {
            for (auto& s : splatters) {
                if (s.active) {
                    s.anim.Draw(s.position, s.flipH);
                }
            }
        }

};