#pragma once
#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <memory>

#include "Crow.hpp"
#include "Goblin.hpp"
#include "Orc.hpp"
#include "Input.hpp"
#include "CameraManager.hpp"
#include "HUD.hpp"

class Game {
    private:
        // Constants
        const int SCREEN_WIDTH = 1600;
        const int SCREEN_HEIGHT = 900;

        // Systems
        CameraManager camera;
        Input input;
        HUD hud;

        // Entities
        Crow crow;
        Experience xp;
        std::vector<Character*> renderQueue;
        std::vector<std::unique_ptr<Mob>> mobs; // Smart Pointer replaces raw pointers (no manual deleting)


        // Spatial grid
        // Grid maps a cell ID to a list of character (mob) indices
        const int CELL_SIZE = 200;
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
        float dt;
        float gameTime = 0.0f;
        float hitStopTimer = 0.0f;  // Freeze on hits
        float timeScale = 1.0f;     // Game speed

        // Difficulty variables
        float goblinSpawnBase = 0.0f;   // Base goblin spawn rate (lower is faster)
        float goblinFactor = 0.05f;     // Factor = how quickly the spawn rate increases
        float orcSpawnBase = 0.0f;
        float orcFactor = 0.05f; 
        float goblinSpawnTimer = 0.0f;
        float orcSpawnTimer = 0.0f;

        // Spawn a mob at random spot relative to player
        template <typename T>   // Generic allowing spawning for every mob
        void SpawnMob(int amount) {
            Vector2 playerPos = crow.GetPosition();
            float left   = playerPos.x - (SCREEN_WIDTH / 2);
            float right  = playerPos.x + (SCREEN_WIDTH / 2);
            float top    = playerPos.y - (SCREEN_HEIGHT / 2);
            float bottom = playerPos.y + (SCREEN_HEIGHT / 2);

            Vector2 squadCenter = {0,0};    // Where the group of mobs spawn
            int buffer = GetRandomValue(-50, 10); // Distance outside the screen
            int side = GetRandomValue(0, 3);
            if (side == 0)      squadCenter = { (float)GetRandomValue(left, right), top + buffer };
            else if (side == 1) squadCenter = { (float)GetRandomValue(left, right), bottom + buffer };
            else if (side == 2) squadCenter = { left + buffer, (float)GetRandomValue(top, bottom) };
            else                squadCenter = { right + buffer, (float)GetRandomValue(top, bottom) };

            // Spawn the group
            for (int i = 0; i < amount; i++) {
                // Create unique pointer
                auto newMob = std::make_unique<T>();

                // Initialize mob
                newMob->init();

                // Add random jitter for mobs to not stack
                Vector2 jitter = { (float)GetRandomValue(-50, 50), (float)GetRandomValue(-50, 50) };

                newMob->SpawnAt(Vector2Add(squadCenter, jitter));

                // Add to list
                mobs.push_back(std::move(newMob));
            }
        }

    public:
        Game() {
            // Initialize the window
            InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Crow Knight");
            SetTargetFPS(60);
        }

        ~Game() {
            crow.Unload();
            Goblin::StaticUnload();
            Orc::StaticUnload();
            hud.Unload();
            CloseWindow();
        }

        void Init() {
            // Setup crow (player)
            crow.Init(Vector2{ (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 });

            // Setup enemies
            Goblin::StaticLoad();
            Orc::StaticLoad();
            mobs.reserve(2000);

            // Reserve space for drawing
            renderQueue.reserve(mobs.size() + 1);

            // Setup systems
            xp.Init();
            camera.Init(SCREEN_WIDTH, SCREEN_HEIGHT, crow.GetPosition());
            hud.Init(SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        void Update() {
            // Handle Hit Stop Timer
            if (hitStopTimer > 0.0f) {
                hitStopTimer -= GetFrameTime(); // Use real time to count down
                dt = 0.0f;                      // Freeze game logic
            } else {
                // Normal time calculation
                dt = GetFrameTime() * timeScale;
            }
            gameTime += dt;
            goblinSpawnTimer += dt;
            orcSpawnTimer += dt;

            // Difficulty Ramp
            // Goblin Logic (Spawns from start, gets faster forever)
            // Formula: Rate = Base / (1 + Time * Factor)
            // Factor = how quickly the spawn rate increases
            float goblinSpawnRate = goblinSpawnBase / (1.0f + (gameTime * goblinFactor));
            
            // Hard Cap: Don't let them spawn faster than 0.1s (prevent crash)
            if (goblinSpawnRate < 0.1f) goblinSpawnRate = 0.1f; 

            if (goblinSpawnTimer > goblinSpawnRate) {
                SpawnMob<Goblin>(1); // Spawn 1 Goblin
                goblinSpawnTimer = 0.0f;
            }

            // Orc Logic (Starts at Minute 3)
            float phaseTwoTime = 100.0f;

            if (gameTime > phaseTwoTime) {
                // Calculate how long we've been in Phase 2
                float timeInPhase2 = gameTime - phaseTwoTime;
                
                // Orcs start slow and get faster
                float orcSpawnRate = orcSpawnBase / (1.0f + (timeInPhase2 * orcFactor));

                // Hard Cap: Don't let hordes spawn faster than every 2 seconds
                if (orcSpawnRate < 2.0f) orcSpawnRate = 2.0f;

                if (orcSpawnTimer > orcSpawnRate) {
                    SpawnMob<Orc>(5); // SPAWN HORDE OF 5
                    orcSpawnTimer = 0.0f;
                }
            }

            // Update Loop
            InputHandler();
            // Update player and camera
            crow.Update(input, dt);
            camera.Update(crow.GetPosition(), crow.GetFaceRight());

            spatialGrid.clear();
            for (size_t i = 0; i < mobs.size(); i++) {
                // Don't grid dead mobs
                if (mobs[i]->GetState() == CharacterState::DEATH) {
                    // Remove gone mobs
                    if (mobs[i]->IsDeadAndGone()) {
                        xp.Spawn(mobs[i]->GetPosition(), 11, mobs[i]->GetXPType());
                        mobs[i] = std::move(mobs.back());   // Don't copy address, actually move the pointer
                        mobs.pop_back();
                        i--;
                        continue; // If gone, move on
                    } 
                    mobs[i]->Update(crow.GetPosition(), false, {0,0,0,0}, {0,0}, &hitStopTimer, dt);    // If dead but not gone
                    continue; // If dead, move on
                }

                // Populate the grid (put every mob in a bucket)
                // Don't grid far away mobs
                if (Vector2DistanceSqr(crow.GetPosition(), mobs[i]->GetPosition()) < 1500.0f * 1500.0f) {
                    int key = GetGridKey(mobs[i]->GetPosition());
                    spatialGrid[key].push_back(i);  // unordered_map[int key] = *mob* => [int, mob]
                }
            }

            // Collision and Physics (now updated physics for each populated grid)
            Rectangle attackBox = crow.GetAttackBox();  // Calculate hitbox once per frame
            bool isHitboxActive = (attackBox.width > 0);

            for (size_t i = 0; i < mobs.size(); i++) {
                // Don't update dead mobs (already did)
                if (mobs[i]->GetState() == CharacterState::DEATH) continue;

                // Reset mob push forces;
                mobs[i]->pushForce = {0, 0};

                // Chase logic
                Vector2 toPlayer = Vector2Subtract(crow.GetPosition(), mobs[i]->GetPosition());
                float distToPlayerSqr = Vector2LengthSqr(toPlayer);
                // Calculate chase force
                Vector2 moveDir = Vector2Normalize(toPlayer);
                mobs[i]->pushForce = Vector2Add(mobs[i]->pushForce, Vector2Scale(moveDir, 200.0f));

                // Player repulsion (if too close to player, push back)
                if (distToPlayerSqr < 1600.0f) {     // 40.0f is the 'personal space' radius
                    mobs[i]->pushForce = Vector2Subtract(mobs[i]->pushForce, Vector2Scale(moveDir, 2000.0f)); // Subtract instead of add
                }

                // Grid collision
                // Locate mob
                int gx = static_cast<int>(mobs[i]->GetPosition().x) / CELL_SIZE;
                int gy = static_cast<int>(mobs[i]->GetPosition().y) / CELL_SIZE;

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
                                
                                if (mobs[i]->GetState() == CharacterState::DEATH) continue;  // If neighbor dead, they don't push

                                Vector2 toNeighbor = Vector2Subtract(mobs[i]->GetPosition(), mobs[j]->GetPosition());
                                float distSqrd = Vector2LengthSqr(toNeighbor);

                                float overlapRadius = mobs[i]->radius + mobs[j]->radius;
                                // If neighbors are overlapping (radius * 2)
                                if (distSqrd < overlapRadius * overlapRadius) {
                                    float dist = sqrt(distSqrd);    // Optimization: only use sqrt if actaully colliding
                                    if (dist < 0.1f) dist = 0.1f;

                                    Vector2 push = Vector2Scale(toNeighbor, 1.0f / dist);    // Create a vector pointing away from neighbor (same as normalizing)

                                    float strength = (overlapRadius - dist) / overlapRadius;    // Closer = stronger push
                                    mobs[i]->pushForce = Vector2Add(mobs[i]->pushForce, Vector2Scale(push, strength * 10000.0f));
                                }

                            }
                    }
                }
                int oldMobHP = mobs[i]->health;  // For crow healing after update

                // Pass in calculated forcel
                mobs[i]->Update(crow.GetPosition(), isHitboxActive, attackBox, mobs[i]->pushForce, &hitStopTimer, dt);

                // Damage and heal logic
                if (mobs[i]->health < oldMobHP && isHitboxActive && CheckCollisionRecs(attackBox, mobs[i]->GetHitbox())) crow.LifeSteal(mobs[i]->GetLifeStealRate());
                if (mobs[i]->health <= 0) crow.KillPlusOne();
                if (mobs[i]->health > 0 && CheckCollisionRecs(crow.GetHitbox(), mobs[i]->GetHitbox())) crow.TakeDamage(0);
                if (crow.GetState() == CharacterState::DEATH) {
                    ResetGame();
                    return; // to stop current frame's logic
                }
            }
            int xpGained = xp.Update(crow.GetPosition(), crow.GetZPosition(), dt); 
            if (xpGained > 0) crow.GainXP(xpGained);
        }

        void Draw() {
            // Clear old queue
            renderQueue.clear();

            // Populate render queue then sort
            renderQueue.push_back(&crow);
            
            float cullRadiusSqr = 1000.0f * 1000.0f;    // Don't draw enemies off screen
            for (auto& mob : mobs) {
                if (Vector2DistanceSqr(crow.GetPosition(), mob->GetPosition()) < cullRadiusSqr) {
                    renderQueue.push_back(mob.get());   // mob.get() grabs the raw pointer
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

                // Draw orbs
                xp.Draw();
                
                // Draw characters
                for (Character* character : renderQueue) character->Draw();

                // Draw debug boxes
                // if (crow.GetAttackBox().width > 0) DrawRectangleLinesEx(crow.GetAttackBox(), 3, RED);
                // DrawRectangleLinesEx(crow.GetHitbox(), 3, RED);
                // for (auto& goblin: goblins) DrawRectangleLinesEx(goblin.GetHitbox(), 3, RED);

            EndMode2D();

            // Draw UI
            DrawFPS(10, 10);

            // Draw HUD
            hud.Draw(crow.GetBloodPercent(), crow.GetLifeStealPercent(), crow.GetXPPercent(), crow.GetLevel(), crow.GetKillCount(), gameTime);

            EndDrawing();
        }

        void ResetGame() {
            // Reset player
            crow.Reset(Vector2{ (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 });

            // Clear enemies
            mobs.clear();
            xp.Reset();

            // Reset Game State
            gameTime = 0.0f;
            goblinSpawnTimer = 0.0f;
            orcSpawnTimer = 0.0f;
            hitStopTimer = 0.0f;
            
            // Reset Camera and Hud
            camera.Init(SCREEN_WIDTH, SCREEN_HEIGHT, crow.GetPosition());
            hud.Init(SCREEN_WIDTH, SCREEN_HEIGHT);
            
            // Clear Spatial Grid
            spatialGrid.clear();
        }

        void InputHandler() {
             // Time management
            if (IsKeyPressed(KEY_MINUS)) timeScale -= 0.1f;
            if (IsKeyPressed(KEY_EQUAL)) timeScale += 0.1f;
            if (IsKeyPressed(KEY_ZERO))  timeScale = 0.0f;
            if (IsKeyPressed(KEY_R))     timeScale = 1.0f;
            if (timeScale < 0.0f) timeScale = 0.0f;

            // Handle player inputs
            input.moveX = (IsKeyDown(KEY_D) ? 1.0f : 0.0f) - (IsKeyDown(KEY_A) ? 1.0f : 0.0f);  // Right/Left
            input.moveY = (IsKeyDown(KEY_S) ? 1.0f : 0.0f) - (IsKeyDown(KEY_W) ? 1.0f : 0.0f);  // Down/Up
            input.jumped = IsKeyPressed(KEY_SPACE);
            input.attacked = IsKeyPressed(KEY_J);
            input.special = IsKeyPressed(KEY_L);
            input.dashed = IsKeyPressed(KEY_I);
            if (IsKeyPressed(KEY_U)) camera.SwitchCamera(crow.GetPosition());
        }

};
