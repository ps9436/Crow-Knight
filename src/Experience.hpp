#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <map>
#include "Animation.hpp"

enum class XPType {
    NONE,
    SMALL,
    MEDIUM,
    LARGE
};

struct XPOrb {
    XPType type;

    Vector2 position;   // XY postion
    float z;            // Z postion

    Vector2 velocity;
    float zVelocity;

    bool isMagnetized;
    int xpValue;
    float animTimer;

    float pickupDelay;
};

class Experience {
    private:
        std::vector<XPOrb> orbs;
        std::map<XPType, Animation> orbAnims;
        XPType type = XPType::NONE;

        Texture2D smallOrb;
        Texture2D mediumOrb;
        Texture2D largeOrb;
        Texture2D shadowOrb;

        Animation orbSmall;
        Animation orbMedium;
        Animation orbLarge;

        const float GRAVITY = 50.0f;
        const float BOUNCE_DAMPING = 0.4f;  // Height lost on bounce
        const float FRICTION = 0.96f;       // Air resistance
        const float MAGNET_FORCE = 4500.0f;
        const float SWIRL_FORCE = 1000.0f;
        const float PICKUP_RADIUS = 200.0f;

    public:
        static constexpr Color PERSIAN_BLUE = { 28, 57, 187, 255 };

        void Init(int scale = 1) {
            smallOrb = LoadTexture("assets/orbs/small-orb-Sheet.png");
            mediumOrb = LoadTexture("assets/orbs/medium-orb-Sheet.png");
            largeOrb = LoadTexture("assets/orbs/large-orb-Sheet.png");
            shadowOrb = LoadTexture("assets/orbs/orb-Shadow.png");

            orbSmall.Init(smallOrb, 1, 1, scale);
            orbMedium.Init(mediumOrb, 1, 1, scale);
            orbLarge.Init(largeOrb, 8, 12, scale);

            orbAnims[XPType::SMALL] = orbSmall;
            orbAnims[XPType::MEDIUM] = orbMedium;
            orbAnims[XPType::LARGE] = orbLarge;
        }

        void Unload() {
            UnloadTexture(smallOrb);
            UnloadTexture(mediumOrb);
            UnloadTexture(largeOrb);
            UnloadTexture(shadowOrb);
        }

        void Spawn(Vector2 pos, int amount, XPType xpType) {
            // Spawn multiple orbs depending on amount
            int orbCount = (amount > 10) ? 1 : 0;
            int valPerOrb = amount / orbCount;

            for (int i = 0; i < orbCount; i++) {
                XPOrb orb;
                orb.type = xpType;
                orb.position = pos;
                orb.z = 10.0f;
                orb.xpValue = valPerOrb;
                orb.isMagnetized = false;
                orb.animTimer = GetRandomValue(0, 100) / 10.0f;
                orb.pickupDelay = 5.0f; // Can't be picked up for 0.5 sec

                // Orb drops
                float angle = GetRandomValue(0, 360) * DEG2RAD;
                float speed = GetRandomValue(100, 300);
                orb.velocity = { cosf(angle) * speed, sinf(angle) * speed};
                orb.zVelocity = GetRandomValue(5, 20);;

                orbs.push_back(orb);
            }
        }

        int Update(Vector2 playerPos, float playerZ, float dt) {
            int xpGained = 0;

            // Update animations
            for (auto& [type, anim] : orbAnims) {
                if (type == XPType::NONE) continue;
                anim.Update(dt);
            }

            for (int i = 0; i < orbs.size(); i++) {
                // Magnetism and Swirling
                float distSqrd = Vector2DistanceSqr(playerPos, orbs[i].position);

                // Trigger magnet
                if (distSqrd < PICKUP_RADIUS * PICKUP_RADIUS) orbs[i].isMagnetized = true;

                if (orbs[i].isMagnetized) {
                    // Reduce pickup delay
                    if (orbs[i].pickupDelay > 0) orbs[i].pickupDelay -= dt;

                    Vector2 toPlayer = Vector2Subtract({playerPos.x, playerPos.y - playerZ}, orbs[i].position);    // Distance and direction to player
                    Vector2 dir = Vector2Normalize(toPlayer);   // Direction of player from orb

                    // Attraction Force to Player
                    float pullStrength = MAGNET_FORCE;
                    orbs[i].velocity = Vector2Add(orbs[i].velocity, Vector2Scale(dir, pullStrength * dt));  // Closer = harder pull

                    // Tangential Force
                    if (distSqrd > 50.0f * 50.0f) {
                        // Vector orthogonal to the Normal component
                        Vector2 tangent = { -dir.y, dir.x };

                        // Add extra force to tangential component for chaotic (aggressive) following
                        float swirlDir = (i % 2 == 0) ? 1.0f : -1.0f;

                        // Increase swirl force when close to keep orbs spinning fast
                        float dynamicSwirl = SWIRL_FORCE;
                        if (distSqrd < 60.0f * 60.0f) dynamicSwirl *= 1.5f;
                        orbs[i].velocity = Vector2Add(orbs[i].velocity, Vector2Scale(tangent, dynamicSwirl * swirlDir * dt));

                        // If orb is too close, push back out
                        if (distSqrd < 40.0f * 40.0f) {
                            float pushForce = 200000.0f; // weird ahh value
                            // -dir to push away from player
                            orbs[i].velocity = Vector2Add(orbs[i].velocity, Vector2Scale(dir, -pushForce * dt));
                        }

                        // Z-axis oscillation
                        float zOscillation = sinf((i * 132)) * 25.0f;
                        float zTarget = 15.0f + zOscillation;   // 15 is near chest of crow
                        // Spring physics
                        float zDiff = zTarget - orbs[i].z;
                        float springForce = 15.0f;
                        orbs[i].zVelocity += zDiff * springForce * dt;
                        orbs[i].zVelocity *= FRICTION;  // Eventuall get to target
                    }

                    // Only collect if delay is gone AND close enough
                    if (orbs[i].pickupDelay <= 0 || distSqrd < 5.0f * 5.0f) {
                        xpGained += orbs[i].xpValue;
                        orbs[i] = orbs.back();
                        orbs.pop_back();
                        i--;
                        continue;
                    }
                } else orbs[i].zVelocity -= GRAVITY * dt;   // Not magnetized (fall to ground)
                // Update Z Position
                orbs[i].z += orbs[i].zVelocity * dt;

                // Update XY Position
                orbs[i].position = Vector2Add(orbs[i].position, Vector2Scale(orbs[i].velocity, dt));

                // Floor collision (gentle settling)
                if (orbs[i].z <= 0.0f) {
                    orbs[i].z = 0.0f;
                    orbs[i].zVelocity *= -BOUNCE_DAMPING; // Bounce back up very slightly
                    if (fabs(orbs[i].zVelocity) < 20.0f) orbs[i].zVelocity = 0.0f; // Stop bouncing if slow
                    orbs[i].velocity = Vector2Scale(orbs[i].velocity, 0.90f); // Friction
                } else {
                    // Air Friction
                    orbs[i].velocity = Vector2Scale(orbs[i].velocity, FRICTION);
                    // Z-drag for the feather fall effect
                    orbs[i].zVelocity *= 0.99f; 
                }
                // Animation Timer
                orbs[i].animTimer += dt;
            }
            return xpGained;
        }

        void Draw() {
            for (const auto& orb : orbs) {
                if (orb.type == XPType::NONE) continue;

                // Draw shadow
                float shadowScale = 1.0f - (orb.z / 200.0f);
                if (shadowScale < 0.2f) shadowScale = 0.2f;

                Rectangle source = { 0, 0, (float)shadowOrb.width, (float)shadowOrb.height };
                Rectangle shadowDest = {
                orb.position.x, 
                orb.position.y, 
                shadowOrb.width * shadowScale * 1,  // 1 is the sprite scale from init
                shadowOrb.height * shadowScale * 1 
                };
                DrawTexturePro(shadowOrb, source, shadowDest, {shadowDest.width/2, shadowDest.height/2}, 0.0f, WHITE);

                Vector2 drawPos = { orb.position.x, orb.position.y - orb.z};
                orbAnims[orb.type].Draw(drawPos);
            }

            // Glow effect
            BeginBlendMode(BLEND_ADDITIVE);
            for (const auto& orb : orbs) {
                Vector2 drawPos = { orb.position.x, orb.position.y - orb.z};
                
                // Calculate sparkle pulse
                // Uses global time + orb's unique offset to create desynchronized pulsing
                float pulse = sinf(GetTime() * 8.0f + orb.animTimer * 5.0f); 
                float alpha = (pulse * 0.3f + 0.7f); // Oscillates between 0.4 and 1.0
                
                Color glowColor = WHITE;
                glowColor.a = (unsigned char)(10.0f * alpha); // Base alpha 100, modulated by pulse

                // Draw the orb again, slightly larger (1.2x), with additive blending
                orbAnims[orb.type].DrawScaled(drawPos, 1.2f, false, glowColor);
            }
            EndBlendMode();
        }

        void Reset() { 
            orbs.clear();
        }
};