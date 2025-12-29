#include "Game.hpp"

int main() {
    Game game;

    // Initialize assets
    game.Init();

    // Main Game Loop
    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ENTER)) {
            game.ResetGame();
        }
        game.Update();
        game.Draw();
        // Timer
        DrawText(TextFormat("Time: %i : %i", (int) GetTime()*60, (int) GetTime() % 60), 10, 70, 20, DARKGRAY);
    }

    return 0;
}