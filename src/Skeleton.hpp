#pragma once
#include <raylib.h>
#include "Mob.hpp"
#include "Input.hpp"

class Skeleton : public Mob {
    private:
        Animation skeletonRUN;
        Animation skeletonHURT;
        Animation skeletonDEATH;
        Animation skeletonWake;

    public:
        static inline Texture2D runSkeleton;
        static inline Texture2D deadSkeleton;
        static inline Texture2D wakeSkeleton;
        static inline Texture2D shadowSkeleton;

        Skeleton () {
            this->shadow = shadowSkeleton;
        }

        // Load skeleton textures
        static void StaticLoad() {
            Skeleton::runSkeleton = LoadTexture("assets/skeleton/skeleton-run-Sheet.png");
            Skeleton::deadSkeleton = LoadTexture("assets/skeleton/skeleton-death-Sheet.png");
            Skeleton::wakeSkeleton = LoadTexture("assets/skeleton/skeleton-wake-Sheet.png");
            Skeleton:shadowSkeleton = LoadTexture("assets/Shadow.png");
        }

        static void StaticUnload() {
            UnloadTexture(runSkeleton);
            UnloadTexture(deadSkeleton);
            UnloadTexture(wakeSkeleton);
        }

        void init(int scale = 3) {
            // Stats (unique to skeleton)
            size = scale;
            speed = 300.0f;
            health = 1;
            weight = 1.5f;
            radius = 15.0f;
            damageRate = 10.0f;
            lifestealRate = 0.2f;
            spawnAnim = true;

            skeletonRUN.Init(runSkeleton, 4, 12.0f, scale);
            animations[CharacterState::RUN] = skeletonRUN;
            skeletonDEATH.Init(deadSkeleton, 8, 12.0f, scale, false);
            animations[CharacterState::DEATH] = skeletonDEATH;
            skeletonWake.Init(wakeSkeleton, 4, 4.0f, scale, false);
            animations[CharacterState::IDLE] = skeletonWake;

            currentState = CharacterState::IDLE;
        }

        Rectangle GetHitbox() override {
            float offX = faceRight ? 3.0f : 12.0f;
            float offY = 11.0f;
            return { position.x - (offX * size), position.y - (offY * size), 16 * size, 21 * size };
        }

        int GetXPValue() override {
            return 0;
        }

        void OnDeathEffect(Vector2 playerPos) override {
            Blood::SpawnBlood(this->position, playerPos, this->size);
        }
};