#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>

#include "Crow.hpp"
#include "goblin.hpp"
#include "Input.hpp"
#include "CameraManager.hpp"
#include "HUD.hpp"
// #include "Particles.hpp"

class Game {
    private:
        // Constants
        const int SCREEN_WIDTH = 1600;
        const int SCREEN_HEIGHT = 900;

        // Systems
        CameraManager camera;
        // Particles particles;
        Input input;
        HUD hud;

        // Entities
        std::vector<Character*> renderQueue;
        Crow crow;
        std::vector<Goblin> goblins;

        // Spatial grid
        const int CELL_SIZE = 200;
        // Grid maps a cell ID to a list of character (mob) indices
        std::unordered_map<int, std::vector<int>> spatialGrid;

        // Generate unique ID for each grid square
        int GetGridKey(Vector2 pos) {
            int gx = static_cast<int>(pos.x) / CELL_SIZE;
            int gy = static_cast<int>(pos.y) / CELL_SIZE;
            return gx + (gy * 10000);
        }

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
        float spawnTimer = 0.0f;    // Linked to spawn rate
        float spawnRate = 0.0f;     // Spawn rate
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
            // Setup crow (player)
            crow.Init(Vector2{ (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 });

            // Setup goblins (enemies)
            Goblin::StaticLoad();
            goblins.reserve(2000);

            // Reserve space for drawing
            renderQueue.reserve(2001);

            // Setup systems
            camera.Init(SCREEN_WIDTH, SCREEN_HEIGHT, crow.GetPosition());
            hud.Init(SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        void Unload() {
            crow.Unload();
            Goblin::StaticUnload();
            goblins.clear();
            hud.Unload();
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
            // if (input.dashed) particles.Spawn(crow.GetPosition(), 5, DARKGRAY); // Feathers

            camera.Update(crow.GetPosition(), crow.GetFaceRight());

            // Spawner
            spawnTimer += GetFrameTime();
            if (spawnTimer > spawnRate) {
                Goblin goblin;
                goblin.Spawn(crow.GetPosition(), SCREEN_WIDTH, SCREEN_HEIGHT);
                goblin.init();
                goblins.push_back(goblin);
                spawnTimer = 0.0f;
            }

            spatialGrid.clear();
            // Populate the grid (put every goblin in a bucket)
            for (size_t i = 0; i < goblins.size(); i++) {
                if (goblins[i].GetState() == CharacterState::DEATH) continue;

                // Don't grid far away goblins
                if (Vector2DistanceSqr(crow.GetPosition(), goblins[i].GetPosition()) < 1500.0f * 1500.0f) {
                    int key = GetGridKey(goblins[i].GetPosition());
                    spatialGrid[key].push_back(i);  // unordered_map[int key] = goblin => [int, mob]
                }
            }

            // Collision and physics
            // Reset push forces
            for (auto& goblin : goblins) goblin.pushForce = {0, 0};

            for (size_t i = 0; i < goblins.size(); i++) {
                // Remove gone goblins
                if (goblins[i].GetState() == CharacterState::DEATH) {
                    if (goblins[i].IsDeadAndGone()) {
                        goblins[i] = goblins.back();
                        goblins.pop_back();
                        i--;
                    } continue; // If dead, move on
                }
                // Chase logic
                Vector2 toPlayer = Vector2Subtract(crow.GetPosition(), goblins[i].GetPosition());
                float distToPlayerSqr = Vector2LengthSqr(toPlayer);
                // Calculate chase force
                Vector2 moveDir = Vector2Normalize(toPlayer);
                goblins[i].pushForce = Vector2Add(goblins[i].pushForce, Vector2Scale(moveDir, 200.0f));

                // Player repulsion (if too close to player, push back)
                if (distToPlayerSqr < 1600.0f) {     // 40.0f is the 'personal space' radius
                    goblins[i].pushForce = Vector2Subtract(goblins[i].pushForce, Vector2Scale(moveDir, 2000.0f)); // Subtract instead of add
                }

                // Grid collision
                // Locate goblin
                int gx = static_cast<int>(goblins[i].GetPosition().x) / CELL_SIZE;
                int gy = static_cast<int>(goblins[i].GetPosition().y) / CELL_SIZE;

                // Check cell and 8 surrounding
                for (int x = -1; x <= 1; x++) {
                    for (int y = -1; y <= 1; y++) {
                        int neighborKey = (gx + x) + ((gy + y) * 10000);
                            // Look up cell in unordered_map (spatialGrid)
                            auto cellIter = spatialGrid.find(neighborKey);
                            if (cellIter == spatialGrid.end()) continue;    // Cell is empty (no mobs)

                            const std::vector<int>& neighbors = cellIter->second;   // Get the list inside the cell (bucket)
                            for (int j : neighbors) {
                                if (i == j) continue;   // Don't push againts self
                                
                                if (goblins[j].GetState() == CharacterState::DEATH) continue;  // If neighbor dead, they don't push

                                Vector2 toNeighbor = Vector2Subtract(goblins[i].GetPosition(), goblins[j].GetPosition());
                                float distSqrd = Vector2LengthSqr(toNeighbor);

                                float overlapRadius = goblins[i].radius * 2.0f;
                                // If neighbors are overlapping (radius * 2)
                                if (distSqrd < overlapRadius * overlapRadius) {
                                    float dist = sqrt(distSqrd);    // Optimization: only use sqrt if actaully colliding
                                    if (dist < 0.1f) dist = 0.1f;

                                    Vector2 push = Vector2Scale(toNeighbor, 1.0f / dist);    // Create a vector pointing away from neighbor (same as normalizing)

                                    float strength = (overlapRadius - dist) / overlapRadius;    // Closer = stronger push
                                    goblins[i].pushForce = Vector2Add(goblins[i].pushForce, Vector2Scale(push, strength * 10000.0f));
                                }

                            }
                    }
                }
            }
            // Update goblins
            Rectangle attackBox = crow.GetAttackBox();  // Calculate hitbox once per frame
            bool isHitboxActive = (attackBox.width > 0);
            for (size_t i = 0; i < goblins.size(); i++) {

                int oldgoblinHP = goblins[i].health;  // For crow healing after update

                // Pass in calculated force
                goblins[i].Update(crow.GetPosition(), isHitboxActive, attackBox, goblins[i].pushForce, &hitStopTimer, dt);

                // Damage and heal logic
                if (goblins[i].health < oldgoblinHP && isHitboxActive && CheckCollisionRecs(attackBox, goblins[i].GetHitbox())) crow.Heal(0.5f);    // 1 is heal factor for killing goblins
                if (goblins[i].health > 0 && CheckCollisionRecs(crow.GetHitbox(), goblins[i].GetHitbox())) crow.TakeDamage(20.0f);   // goblins hit 20 per second
                if (crow.GetState() == CharacterState::DEATH) {
                    ResetGame();
                    return;
                }
                // Check Hit (Visual Effects)
                // if (isHitboxActive && goblins[i].immunityTimer <= 0.0f && CheckCollisionRecs(attackBox, goblins[i].GetHitbox())) {
                //     particles.Spawn(goblins[i].GetPosition(), 15, MAROON); // Spawn blood
                //     if (goblins[i].health <= 1) particles.Spawn(goblins[i].GetPosition(), 30, RED); // If killed, bigger blood
                // }
            }

            // particles.Update(GetFrameTime());
        }

        void Draw() {
            // Clear old queue
            renderQueue.clear();

            // Populate render queue then sort
            renderQueue.push_back(&crow);
            
            float cullRadiusSqr = 1000.0f * 1000.0f;    // Don't draw enemies off screen
            for (auto& goblin : goblins) {
                if (Vector2DistanceSqr(crow.GetPosition(), goblin.GetPosition()) < cullRadiusSqr) {
                    renderQueue.push_back(&goblin);
                }
            }
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
                // for (auto& goblin: goblins) DrawRectangleLinesEx(goblin.GetHitbox(), 3, RED);

            EndMode2D();

            // Draw UI
            DrawFPS(10, 10);
            DrawText(TextFormat("Enemies: %i", goblins.size()), 10, 40, 20, DARKGRAY);

            // Draw HUD
            hud.Draw(crow.GetBloodPercent());

            EndDrawing();
        }

        void ResetGame() {
            // Reset player
            crow.Reset(Vector2{ (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 });

            // Clear enemies
            goblins.clear();

            // Reset Game State
            spawnTimer = 0.0f;
            hitStopTimer = 0.0f;
            
            // Reset Camera
            camera.Init(SCREEN_WIDTH, SCREEN_HEIGHT, crow.GetPosition());
            
            // Clear Spatial Grid
            spatialGrid.clear();
        }

};
