#pragma once
#include <raylib.h>

class HUD {
    private:
        Texture2D bloodFrame;
        Texture2D bloodFill;
        Vector2 position;
        float scale;

    public:
        void Init(int screenWidth, int screenHeight) {
            // bloodFrame = LoadTexture();
            // bloodFill = LoadTexture();

            position = { (float)screenWidth/2, 20.0f };
            scale = 4.0f;
        }

        void Unload() {
            // UnloadTexture(bloodFrame);
            // UnloadTexture(bloodFill);
        }

        void Draw(float bloodPercent) {
            // Shadow
            DrawRectangle(position.x, position.y, 300, 30, BLACK);
            // Empty bar
            DrawRectangle(position.x, position.y, 300, 40, DARKGRAY);

            // Draw fill (blood)
            float fillWidth = 300.0f * bloodPercent;
            Color bloodColor = (bloodPercent < 0.2f) ? RED : MAROON;
            DrawRectangle(position.x, position.y, (int)fillWidth, 40, bloodColor);

            // Draw frame
            DrawRectangleLines(position.x, position.y, 300, 40, BLACK);

            // Text Label
            DrawText("BLOOD", position.x + 10, position.y + 10, 20, RAYWHITE);

            

            /* Once you have sprites, replace the Rectangles with:
           DrawTextureEx(barFrame, position, 0.0f, scale, WHITE);
           
           // For the fill, you'd use a source rectangle to crop the image
           Rectangle source = {0, 0, barFill.width * bloodPercent, barFill.height};
           DrawTextureRec(barFill, source, position, WHITE);
            */
        }
};