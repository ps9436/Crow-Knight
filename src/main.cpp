#include <raylib.h>
#include <vector>
#include <algorithm>
#include "Crow.hpp"
#include "Input.hpp"
#include "CameraManager.hpp"
#include "Mob.hpp"
#include "Owl.hpp"

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
    const int SCREEN_WIDTH = 1280;
    const int SCREEN_HEIGHT = 720;

    // Initialize the window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Crow Knight");
    SetTargetFPS(60);

    // Crow
    Crow crow;
    crow.Init(Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT/2});

    // Owl
    Owl::runOWL = LoadTexture("assets/Oscar-Run-Sheet.png");
    Owl::revrunOWL = LoadTexture("assets/Oscar-Reverse-Run-Sheet.png");
    Owl::hurtOWL = LoadTexture("assets/Oscar-Hurt-Sheet.png");
    Owl::deadOWL = LoadTexture("assets/Charles-Feather-Sheet.png");
    Owl::shadowOWL = LoadTexture("assets/Charles-Shadow.png");
    std::vector<Owl> owls;
    float spawnTimer = 0.0f;    // Linked to spawnRate
    
    // Initialize camera
    CameraManager camera;
    camera.Init(SCREEN_WIDTH, SCREEN_HEIGHT, crow.GetPosition());

    Input input{};    

    // Main game loop
    while (!WindowShouldClose()) {

        input.moveX = (IsKeyDown(KEY_D) ? 1.0f : 0.0f) - (IsKeyDown(KEY_A) ? 1.0f : 0.0f);  // Right/Left
        input.moveY = (IsKeyDown(KEY_S) ? 1.0f : 0.0f) - (IsKeyDown(KEY_W) ? 1.0f : 0.0f);  // Down/Up
        input.jumped = IsKeyPressed(KEY_SPACE);
        input.attacked = IsKeyPressed(KEY_J);
        input.special = IsKeyPressed(KEY_E);
        input.dashed = IsKeyPressed(KEY_I);
        if (IsKeyPressed(KEY_U)) camera.SwitchCamera(crow.GetPosition());

        crow.Update(input);
        camera.Update(crow.GetPosition(), crow.GetFaceRight());

        // Mob spawner
        float spawnRate = 1.0f;     // Owl spawn rate
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

        Rectangle attackBox = {0,0,0,0};
        if (input.attacked) attackBox = crow.GetAttackBox();

        // Update owls with forces
        int index = 0;
        for (auto& owl : owls) {    // Modify the original mob (auto&)
            owl.Update(crow.GetPosition(), input.attacked, attackBox, pushForces[index]);
            index++;
            // if (CheckCollisionRecs(crow.GetHitbox(), owl.GetHitbox())) {
            // // PlayerTakeDamage();
            // }
        }

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
            // // Debug: Draw attack box so you can see where you are hitting
            // if (input.attacked) DrawRectangleLinesEx(attackBox, 3, RED);
            // DrawRectangleLinesEx(crow.GetHitbox(), 3, RED);
            // for (auto& owl: owls) DrawRectangleLinesEx(owl.GetHitbox(), 3, RED);

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