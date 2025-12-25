#include <raylib.h>
#include "Crow.hpp"
#include "Input.hpp"

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 800;

    // Initialize the window
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Crow Knight");
    SetTargetFPS(60);

    Input input{};

    Crow crow;
    crow.Init(Vector2{SCREEN_WIDTH/2, SCREEN_HEIGHT/2});
    
    // Main game loop
    while (!WindowShouldClose()) {

        input.moveX = (IsKeyDown(KEY_D) ? 1.0f : 0.0f) - (IsKeyDown(KEY_A) ? 1.0f : 0.0f);  // Right/Left
        input.moveY = (IsKeyDown(KEY_S) ? 1.0f : 0.0f) - (IsKeyDown(KEY_W) ? 1.0f : 0.0f);  // Down/Up

        input.jumped = IsKeyPressed(KEY_SPACE);
        input.attacked = IsKeyPressed(KEY_J);
        input.special = IsKeyPressed(KEY_E);
        input.dashed = IsKeyPressed(KEY_I);

        crow.Update(input);
        
        BeginDrawing();
        ClearBackground(RAYWHITE);

        crow.Draw();

        EndDrawing();
    }

    crow.Unload();

    CloseWindow();
    return 0;
}