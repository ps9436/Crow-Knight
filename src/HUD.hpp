#pragma once
#include <raylib.h>

class HUD {
    private:
        Texture2D bloodFrame;
        Texture2D bloodFill;
        Vector2 position;
        float scale;
        int screenWidth;

    public:
        void Init(int sWidth, int sHeight) {
            bloodFrame = LoadTexture("assets/hud/blood-border.png");
            bloodFill = LoadTexture("assets/hud/blood-fill.png");

            position = { 0.0f, 0.0f };
            scale = 4.0f;
            screenWidth = sWidth;
        }

        void Unload() {
            UnloadTexture(bloodFrame);
            UnloadTexture(bloodFill);
        }

        void Draw(float bloodPercent) {
            // Centering
            float frameCenter = (screenWidth - bloodFrame.width*scale)/2.0f;
            float fillCenter = (screenWidth - bloodFill.width*scale)/2.0f;

            // Blood frame
            Rectangle frameSource = { 0.0f, 0.0f, (float)bloodFrame.width, (float)bloodFrame.height };
            Rectangle frameDest = { frameCenter, position.y, (float)bloodFrame.width*scale, (float)bloodFrame.height*scale };
            DrawTexturePro(bloodFrame, frameSource, frameDest, {0.0f, 0.0f}, 0.0f, WHITE);

            // Blood fill
            float currentBlood = bloodFill.width * bloodPercent;    // Only grab what is remaining, not the entire blood
            Rectangle fillSource = { 0.0f, 0.0f, (float)currentBlood, (float)bloodFill.height };
            Rectangle fillDest = { fillCenter, position.y, (float)currentBlood*scale, (float)bloodFill.height*scale };
            DrawTexturePro(bloodFill, fillSource, fillDest, {0.0f, 0.0f}, 0.0f, WHITE);
        }
};