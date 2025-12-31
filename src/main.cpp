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
    }

    return 0;
}