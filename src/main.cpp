#include "Game.hpp"

int main() {
    Game game;

    // Initialize assets
    game.Init();

    // Main Game Loop
    while (!WindowShouldClose()) {
        game.Update();
        game.Draw();
    }

    return 0;
}