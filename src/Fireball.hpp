#pragma once
#include "Projectile.hpp"
#include "Crow.hpp"

class Fireball : public Projectile {
    private:
        float radius = 15.0f;   // 5.0f times sprite size
    public:
        static inline Texture2D fireballTex;

        static void StaticLoad() { fireballTex = LoadTexture("assets/Sprite-0002-Sheet.png"); }
        static void StaticUnload() { UnloadTexture(fireballTex); }

        Fireball(Vector2 start, Vector2 end) 
            : Projectile(start, end, 400.0f, 15.0f, 0.0f) {
            animation.Init(fireballTex, 12, 12.0f, 3.0f, true); // 3.0f is sprite size
        }

        void OnHit(Character& target) override {
            // Special Effect (Cast to Crow)
            Crow* player = dynamic_cast<Crow*>(&target);
            if (player) {
                player->TakeInstantDamage(0);
                player->ApplyBloodRot(); 
            }
        }
};