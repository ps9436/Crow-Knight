#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <iostream>

#include "Crow.hpp"
#include "Owl.hpp"
#include "Input.hpp"
#include "CameraManager.hpp"
#include "Particles.hpp"

class Game {
    private:
        // Constants
        const int SCREEN_WIDTH = 1600;
        const int SCREEN_HEIGHT = 900;

        // Systems
        CameraManager camera;
        Particles particles;
        Input input;

        // Entities
        Crow crow;
        std::vector<Owl> owls;

        // Debug grid
        void DrawDebugGrid(int extent, int spacing) {
            // extent = how far the grid goes in each direction
            // spacing = the size of each tile
            int spacingX = spacing;
            int spacingY = spacing / 2;

            // Draw Vertical Lines
            for (int x = -extent; x <= extent; x += spacingX) {
                // Draw lighter lines for the grid
                Color color = (x == 0) ? BLACK : LIGHTGRAY; // Make center line Black
                DrawLine(x, -extent, x, extent, color);
            }
            // Draw Horizontal Lines
            for (int y = -extent; y <= extent; y += spacingY) {
                Color color = (y == 0) ? BLACK : LIGHTGRAY; // Make center line Black
                DrawLine(-extent, y, extent, y, color);
            }
            // Draw a Circle at absolute center (0,0) for reference
            DrawCircle(0, 0, 5, RED);
        }

        // Game state
        float hitStopTimer = 0.0f;  // Freeze on hits
        float timeScale = 1.0f;     // Game speed
        bool spawnTimer = 2.0f;     // spawn rate
        int currentLevel = 1;       // Level/wave

    public:
        Game() {
            // Initialize the window
            InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Crow Knight");
            SetTargetFPS(60);
        }

        ~Game() {
            Unload();
            CloseWindow();
        }

        void Init() {
            // Setup crow
            crow.Init(Vector2{ (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 });
            
            // Setup camera
            camera.Init(SCREEN_WIDTH, SCREEN_HEIGHT, crow.GetPosition());

            // Load Static Textures
            Owl::StaticLoad();
        }

        void Unload() {
            crow.Unload();
            Owl::StaticUnload();
            owls.clear();
        }

        void Update() {
            // Time management
            if (IsKeyPressed(KEY_MINUS)) timeScale -= 0.1f;
            if (IsKeyPressed(KEY_EQUAL)) timeScale += 0.1f;
            if (IsKeyPressed(KEY_ZERO))  timeScale = 0.0f;
            if (IsKeyPressed(KEY_R))     timeScale = 1.0f;
            if (timeScale < 0.0f) timeScale = 0.0f;
            
            float dt;
            // Handle Hit Stop Timer
            if (hitStopTimer > 0.0f) {
                hitStopTimer -= GetFrameTime(); // Use real time to count down
                dt = 0.0f;                      // Freeze game logic
            } else {
                // Normal time calculation
                dt = GetFrameTime() * timeScale;
            }

            // Handle inputs
            input.moveX = (IsKeyDown(KEY_D) ? 1.0f : 0.0f) - (IsKeyDown(KEY_A) ? 1.0f : 0.0f);  // Right/Left
            input.moveY = (IsKeyDown(KEY_S) ? 1.0f : 0.0f) - (IsKeyDown(KEY_W) ? 1.0f : 0.0f);  // Down/Up
            input.jumped = IsKeyPressed(KEY_SPACE);
            input.attacked = IsKeyPressed(KEY_J);
            input.special = IsKeyPressed(KEY_E);
            input.dashed = IsKeyPressed(KEY_I);
            if (IsKeyPressed(KEY_U)) camera.SwitchCamera(crow.GetPosition());

            // Update player and camera
            crow.Update(input, dt);
            if (input.dashed) particles.Spawn(crow.GetPosition(), 5, DARKGRAY); // Feathers

            camera.Update(crow.GetPosition(), crow.GetFaceRight());

            // Spawner
            float spawnRate = 0.2f;
            spawnTimer += GetFrameTime();
            if (spawnTimer > spawnRate) {
                Owl owl;
                owl.Spawn(crow.GetPosition(), SCREEN_WIDTH, SCREEN_HEIGHT);
                owl.init();
                owls.push_back(owl);
                spawnTimer = 0.0f;
            }

            // Collision and physics

            // Cleanup dead
            owls.erase(std::remove_if(owls.begin(), owls.end(), [](Owl& o) {
                return o.IsDeadAndGone();
            }), owls.end());

            // Calculate forces
            std::vector<Vector2> pushForces(owls.size(), {0, 0});

            // Calculate hitbox once per frame
            Rectangle attackBox = crow.GetAttackBox();
            bool isHitboxActive = (attackBox.width > 0);

            for (size_t i = 0; i < owls.size(); i++) {
                if (owls[i].GetState() == CharacterState::DEATH) continue;
                
                // Player repulsion
                // Check againts player (don't stack on top of player)
                Vector2 toPlayer = Vector2Subtract(crow.GetPosition(), owls[i].GetPosition());
                float distToPlayer = Vector2Length(toPlayer);
                // If too close, push back
                if (distToPlayer < 40.0f) {     // 40.0f is the 'personal space' radius
                    Vector2 push = Vector2Normalize(Vector2Subtract(owls[i].GetPosition(), crow.GetPosition()));
                    pushForces[i] = Vector2Add(pushForces[i], Vector2Scale(push, 2000.0f));
                }
                // Check againts other owls (don't stack on top of eachother)
                for (size_t j = 0; j < owls.size(); j++) {
                    if (i == j) continue;    // Don't push againts self
                    if (owls[j].GetState() == CharacterState::DEATH) continue;  // If neighbor dead, they don't push

                    Vector2 toNeighbor = Vector2Subtract(owls[i].GetPosition(), owls[j].GetPosition());
                    float dist = Vector2Length(toNeighbor);

                    float overlapRadius = owls[i].radius * 2.0f;
                    // If neighbors are overlapping (radius * 2)
                    if (dist < overlapRadius) {
                        Vector2 push = Vector2Normalize(toNeighbor);    // Create a vector pointing away from neighbor

                        float strength = (overlapRadius - dist) / overlapRadius;    // Closer = stronger push
                        pushForces[i] = Vector2Add(pushForces[i], Vector2Scale(push, strength * 500.0f));
                    }
                }
            }

            // Update owls
            for (size_t i = 0; i < owls.size(); i++) {
                // Pass in calculated force
                owls[i].Update(crow.GetPosition(), isHitboxActive, attackBox, pushForces[i], &hitStopTimer, dt);

                // Check Hit (Visual Effects)
                if (isHitboxActive && owls[i].immunityTimer <= 0.0f && CheckCollisionRecs(attackBox, owls[i].GetHitbox())) {
                    particles.Spawn(owls[i].GetPosition(), 15, MAROON); // Spawn blood
                    if (owls[i].health <= 1) particles.Spawn(owls[i].GetPosition(), 30, RED); // If killed, bigger blood
                }
            }

            particles.Update(GetFrameTime());
        }

        void Draw() {
            // Create a render queue then sort
            std::vector<Character*> renderQueue;
            renderQueue.push_back(&crow);
            for (auto& owl : owls) renderQueue.push_back(&owl);

            std::sort(renderQueue.begin(), renderQueue.end(), [](Character* a, Character* b) {
                return a->GetPosition().y < b->GetPosition().y;
            });

            // Drawing
            BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(camera.raylibCam);
                DrawDebugGrid(5000, 100);

                // Draw characters
                for (Character* character : renderQueue) {
                character->Draw();
                }
                // particles.Draw(); // Draw particles after characters

                // Draw debug boxes
                // if (crow.GetAttackBox().width > 0) DrawRectangleLinesEx(crow.GetAttackBox(), 3, RED);
                // for (auto& owl: owls) DrawRectangleLinesEx(owl.GetHitbox(), 3, RED);

            EndMode2D();

            // Draw UI
            DrawFPS(10, 10);
            DrawText(TextFormat("Enemies: %i", owls.size()), 10, 40, 20, DARKGRAY);

            EndDrawing();
        }

};
