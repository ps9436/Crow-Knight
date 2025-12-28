#include <raylib.h>
#include <vector>
#include <algorithm>
#include "Crow.hpp"
#include "Input.hpp"
#include "CameraManager.hpp"
#include "Mob.hpp"
#include "Owl.hpp"
#include "Particles.hpp"

// Static variabels
Texture2D Owl::runOWL;
Texture2D Owl::revrunOWL;
Texture2D Owl::hurtOWL;
Texture2D Owl::deadOWL;
Texture2D Owl::shadowOWL;

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

int main() {
    const int SCREEN_WIDTH = 1600;
    const int SCREEN_HEIGHT = 900;
    float timeScale = 1.0f;
    float hitStopTimer = 0.0f;

    // Initialize the window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Crow Knight");
    SetTargetFPS(60);

    // Crow
    Crow crow;
    crow.Init(Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT/2});
    Input input{};

    // Owl
    Owl::runOWL = LoadTexture("assets/goblin-run.png");
    Owl::revrunOWL = LoadTexture("assets/Oscar-Reverse-Run-Sheet.png");
    Owl::hurtOWL = LoadTexture("assets/goblin-hurt.png");
    Owl::deadOWL = LoadTexture("assets/goblin-death2.png");
    Owl::shadowOWL = LoadTexture("assets/Charles-Shadow.png");
    std::vector<Owl> owls;
    float spawnTimer;    // Linked to spawnRate
    
    // Initialize camera
    CameraManager camera;
    camera.Init(SCREEN_WIDTH, SCREEN_HEIGHT, crow.GetPosition());

    // Particles
    Particles particles;

    // Main game loop
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_MINUS)) timeScale -= 0.1f;
        if (IsKeyPressed(KEY_EQUAL)) timeScale += 0.1f;
        if (IsKeyPressed(KEY_ZERO))  timeScale = 0.0f; // Pause
        if (IsKeyPressed(KEY_R))     timeScale = 1.0f; // Reset
        float dt = GetFrameTime();
        if (timeScale < 0.0f) timeScale = 0.0f;
        // 1. Handle Hit Stop Timer
        if (hitStopTimer > 0.0f) {
            hitStopTimer -= GetFrameTime(); // Use real time to count down
            dt = 0.0f;                      // FREEZE the game logic
        } else {
            // Normal time calculation
            dt = GetFrameTime() * timeScale;
        }

        input.moveX = (IsKeyDown(KEY_D) ? 1.0f : 0.0f) - (IsKeyDown(KEY_A) ? 1.0f : 0.0f);  // Right/Left
        input.moveY = (IsKeyDown(KEY_S) ? 1.0f : 0.0f) - (IsKeyDown(KEY_W) ? 1.0f : 0.0f);  // Down/Up
        input.jumped = IsKeyPressed(KEY_SPACE);
        input.attacked = IsKeyPressed(KEY_J);
        input.special = IsKeyPressed(KEY_E);
        input.dashed = IsKeyPressed(KEY_I);
        if (IsKeyPressed(KEY_U)) camera.SwitchCamera(crow.GetPosition());

        crow.Update(input, dt);
        if (input.dashed) particles.Spawn(crow.GetPosition(), 5, DARKGRAY); // Feathers
        camera.Update(crow.GetPosition(), crow.GetFaceRight());

        // Mob spawner
        float spawnRate = 0.2f;     // Owl spawn rate
        spawnTimer += GetFrameTime();
        if (spawnTimer > spawnRate) {
            Owl owl;
            owl.Spawn(crow.GetPosition(), SCREEN_WIDTH, SCREEN_HEIGHT);
            
            owl.init();
            owls.push_back(owl);
            spawnTimer = 0.0f;
        }

        // Mob loop
        // Remove dead owls
        owls.erase(std::remove_if(owls.begin(), owls.end(), [](Owl& o) { 
                return o.IsDeadAndGone();
            }), owls.end());

        // Calculate repulsion
        std::vector<Vector2> pushForces(owls.size(), {0 ,0});
        // Collisions
        for (int i = 0; i < owls.size(); i++) {
            // If dead, don't get pushed.
            if (owls[i].GetState() == CharacterState::DEATH) continue;

            // Check againts player (don't stack on top of player)
            Vector2 toPlayer = Vector2Subtract(crow.GetPosition(), owls[i].GetPosition());
            float distToPlayer = Vector2Length(toPlayer);
            // If too close, push back
            if (distToPlayer < 40.0f) {     // 40.0f is the 'personal space' radius
                Vector2 push = Vector2Normalize(Vector2Subtract(owls[i].GetPosition(), crow.GetPosition()));
                pushForces[i] = Vector2Add(pushForces[i], Vector2Scale(push, 2000.0f));
            }
            // Check againts other owls (don't stack on top of eachother)
            for (int j = 0; j < owls.size(); j++) {
                if (i == j) continue;    // Don't push againts self
                // If neighbor dead, they don't push
                if (owls[j].GetState() == CharacterState::DEATH) continue;

                Vector2 toNeighbor = Vector2Subtract(owls[i].GetPosition(), owls[j].GetPosition());
                float dist = Vector2Length(toNeighbor);

                float overlapRadius = owls[i].radius * 2.0f;
                // If neighbors are overlapping (radius * 2)
                if (dist < overlapRadius) {
                    // Create a vector pointing away from neighbor
                    Vector2 push = Vector2Normalize(toNeighbor);

                    // Closer = stronger push
                    float strength = (overlapRadius - dist) / overlapRadius;
                    pushForces[i] = Vector2Add(pushForces[i], Vector2Scale(push, strength * 500.0f));
                }
            }
        }

        Rectangle attackBox = crow.GetAttackBox();
        bool isHitboxActive = (attackBox.width > 0);

        // Update owls with forces
        int index = 0;
        for (auto& owl : owls) {    // Modify the original mob (auto&)
            owl.Update(crow.GetPosition(), isHitboxActive, attackBox, pushForces[index], &hitStopTimer, dt);
            index++;
            if (isHitboxActive && owl.immunityTimer <= 0.0f && CheckCollisionRecs(attackBox, owl.GetHitbox())) {
             // Spawn blood
             particles.Spawn(owl.GetPosition(), 15, MAROON); 
             // If killed, bigger blood
             if (owl.health <= 1) particles.Spawn(owl.GetPosition(), 30, RED);
        }
            // if (CheckCollisionRecs(crow.GetHitbox(), owl.GetHitbox())) {
            // // PlayerTakeDamage();
            // }
        }

        particles.Update(GetFrameTime());

        // List of pointer to all characters
        std::vector<Character*> renderQueue;

        // Add the player
        renderQueue.push_back(&crow);
        // Add all the enemies
        for (auto& owl : owls) renderQueue.push_back(&owl);

        // Sort owls by Y position so lower ones draw on top of higher ones
        std::sort(renderQueue.begin(), renderQueue.end(), [](Character* a, Character* b) {
            return a->GetPosition().y < b->GetPosition().y;
        });
        BeginDrawing();

            ClearBackground(RAYWHITE);
            BeginMode2D(camera.raylibCam);
            DrawDebugGrid(5000, 100);
            for (Character* character : renderQueue) {
                character->Draw();
            }
            // particles.Draw(); // Draw particles after characters
            // // Debug: Draw attack box so you can see where you are hitting
            if (attackBox.width > 0) DrawRectangleLinesEx(attackBox, 3, RED);
            // DrawRectangleLinesEx(crow.GetHitbox(), 3, RED);
            for (auto& owl: owls) DrawRectangleLinesEx(owl.GetHitbox(), 3, RED);

        EndMode2D();
        EndDrawing();
    }

    crow.Unload();
    UnloadTexture(Owl::runOWL);
    UnloadTexture(Owl::revrunOWL);
    UnloadTexture(Owl::hurtOWL);
    UnloadTexture(Owl::deadOWL);
    UnloadTexture(Owl::shadowOWL);

    CloseWindow();
    return 0;
}