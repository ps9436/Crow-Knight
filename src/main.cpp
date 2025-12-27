#include <raylib.h>
#include <vector>
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
        Rectangle attackBox = {0,0,0,0};
        if (input.attacked) attackBox = crow.GetAttackBox();

        for (auto& owl : owls) {    // Modify the original mob (auto&)
            owl.Update(crow.GetPosition(), input.attacked, attackBox);
            // if (CheckCollisionRecs(crow.GetHitbox(), owl.GetHitbox())) {
            // // PlayerTakeDamage();
            // }
        }
        
        BeginDrawing();

            ClearBackground(RAYWHITE);
            BeginMode2D(camera.raylibCam);
            DrawDebugGrid(5000, 100);
            for (auto& owl : owls) {
                owl.Draw();
            }
            crow.Draw();
            
            // Debug: Draw attack box so you can see where you are hitting
            if (input.attacked) DrawRectangleLinesEx(attackBox, 3, RED);
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