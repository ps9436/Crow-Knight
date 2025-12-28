#pragma once
#include <raylib.h>
#include <vector>
#include <algorithm>

struct Particle {
    Vector2 position;
    Vector2 velocity;
    float rotation;
    float rotSpeed;
    float lifeTime;
    float maxLife;
    Color color;
    float size;
};

class Particles {
    public:
        std::vector<Particle> particles;

        // Spawn a burst of particles
        void Spawn(Vector2 pos, int count, Color color) {
            for (int i = 0; i < count; i++) {
                Particle p;
                p.position = pos;
                
                // Explosion math
                p.velocity = { 
                    (float)GetRandomValue(-300, 300),   // X
                    (float)GetRandomValue(-500, -100)   // Y
                };

                p.rotation = GetRandomValue(0, 360);
                p.rotSpeed = GetRandomValue(-10, 10);
                
                // Randomize life so they don't all disappear at once
                p.maxLife = (float)GetRandomValue(5, 10) / 10.0f; // 0.5s to 1.0s
                p.lifeTime = p.maxLife;

                p.color = color;
                p.size = (float)GetRandomValue(3, 6); // Pixel size
                
                particles.push_back(p);
            }
        }
        
        // Pass 'GetFrameTime()' for particles to move during Hit Stop.
        // Pass your variable 'dt' for particles to freeze with the game.
        void Update(float dt) {
            for (auto& p : particles) {
                // Apply Gravity (Pull down)
                p.velocity.y += 1500.0f * dt; 

                // Move
                p.position.x += p.velocity.x * dt;
                p.position.y += p.velocity.y * dt;

                // Rotate
                p.rotation += p.rotSpeed;

                // Age
                p.lifeTime -= dt;
            }

            // Remove dead particles (Garbage Collection)
            // Loops backwards to safely remove items from vector
            for (int i = particles.size() - 1; i >= 0; i--) {
                if (particles[i].lifeTime <= 0) {
                    particles.erase(particles.begin() + i);
                }
            }
        }

        void Draw() {
            for (const auto& p : particles) {
                // Calculate Transparency (Alpha)
                // 1.0 (Full life) -> 0.0 (Dead)
                float alpha = p.lifeTime / p.maxLife;
                
                // Raylib helper to fade a color
                Color fadeColor = Fade(p.color, alpha);

                Rectangle rec = { p.position.x, p.position.y, p.size, p.size };
                Vector2 origin = { p.size/2, p.size/2 }; // Rotate around center

                DrawRectanglePro(rec, origin, p.rotation, fadeColor);
            }
        }
};