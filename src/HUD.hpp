#pragma once
#include <raylib.h>

class HUD {
    private:
        Texture2D bloodFrame;
        Texture2D bloodFill;
        Texture2D lifeStealFrame;
        Texture2D lifeStealFill;
        Vector2 position;
        float scale;
        int screenWidth;
        int minutes;
        int seconds;
        Font BoldPixels;

    public:
        void Init(int sWidth, int sHeight) {
            bloodFrame = LoadTexture("assets/hud/blood-border.png");
            bloodFill = LoadTexture("assets/hud/blood-fill.png");
            lifeStealFrame = LoadTexture("assets/hud/lifesteal-border.png");
            lifeStealFill = LoadTexture("assets/hud/lifesteal-fill.png");
            BoldPixels = LoadFontEx("assets/BoldPixels.ttf", 16, 0, 0);
            SetTextureFilter(BoldPixels.texture, TEXTURE_FILTER_POINT);

            position = { 0.0f, 0.0f };
            scale = 4.0f;
            screenWidth = sWidth;
            minutes = 0;
            seconds = 0;
        }

        void Unload() {
            UnloadTexture(bloodFrame);
            UnloadTexture(bloodFill);
            UnloadTexture(lifeStealFrame);
            UnloadTexture(lifeStealFill);
            UnloadFont(BoldPixels);
        }

        void Draw(float bloodPercent, float lifeStolen, float gameTime) {
            // Centering (blood = lifesteal)
            float frameCenter = (screenWidth - bloodFrame.width*scale)/2.0f;
            float fillCenter = (screenWidth - bloodFill.width*scale)/2.0f;

            // Blood frame
            Rectangle frameSource = { 0.0f, 0.0f, (float)bloodFrame.width, (float)bloodFrame.height };
            Rectangle frameDest = { frameCenter, position.y, (float)bloodFrame.width*scale, (float)bloodFrame.height*scale };
            DrawTexturePro(bloodFrame, frameSource, frameDest, {0.0f, 0.0f}, 0.0f, WHITE);

            // Blood fill
            float currentBlood = (bloodFill.width * bloodPercent);    // Only grab what is remaining, not the entire texture
            Rectangle fillSource = { 0.0f, 0.0f, (float)currentBlood, (float)bloodFill.height };
            Rectangle fillDest = { fillCenter, position.y, (float)currentBlood * scale, (float)bloodFill.height*scale };
            DrawTexturePro(bloodFill, fillSource, fillDest, {0.0f, 0.0f}, 0.0f, WHITE);

            // Lifesteal frame
            Rectangle frameLSSource = { 0.0f, 0.0f, (float)lifeStealFrame.width, (float)lifeStealFrame.height };
            Rectangle frameLSDest = { frameCenter, position.y, (float)lifeStealFrame.width*scale, (float)lifeStealFrame.height*scale };
            DrawTexturePro(lifeStealFrame, frameLSSource, frameLSDest, {0.0f, 0.0f}, 0.0f, WHITE);

            // Life stolen
            float currentLifesteal = (bloodFill.width * lifeStolen);  // Life steal texture width
            Rectangle stealSource = { 0.0f, 0.0f, (float)currentLifesteal, (float)lifeStealFill.height };
            Rectangle stealDest = { fillCenter, position.y, (float)currentLifesteal * scale, (float)lifeStealFill.height*scale };
            DrawTexturePro(lifeStealFill, stealSource, stealDest, {0.0f, 0.0f}, 0.0f, WHITE);

            // Timer
            minutes = (int)gameTime / 60;
            seconds = (int)gameTime % 60;
            /* Text timer
            int fontSize = 50;
            int yPos = 48;
            // : is anchor
            const char* colon = ":";
            int colonWidth = MeasureText(colon, fontSize);
            int timerCenter = (screenWidth - colonWidth) / 2;
            DrawText(colon, timerCenter, yPos, fontSize, BLACK);
            // Draw Minutes (Right Aligned to the Colon)
            const char* minText = TextFormat("%02i", minutes);
            int minWidth = MeasureText(minText, fontSize);
            DrawText(minText, timerCenter - minWidth - 300/fontSize, yPos, fontSize, BLACK);
            // Draw Seconds (Left Aligned to the Colon)
            const char* secText = TextFormat("%02i", seconds);
            DrawText(secText, timerCenter + 575/fontSize, yPos, fontSize, BLACK);
            */

            // Font timer
            float fontSize = 50.0f;
            float spacing = 1.0f; // Space between letters
            float yPos = 32.0f;
            Color tint = BLACK;
            Vector2 offsetShadow = { 0, 4 };
            Color tintShadow = RAYWHITE;

            // : is anchor
            const char* colon = ":";
            Vector2 colonSize = MeasureTextEx(BoldPixels, colon, fontSize, spacing);
            Vector2 colonPos = { (screenWidth - colonSize.x) / 2, yPos };
            Vector2 colonShadowPos = {colonPos.x + offsetShadow.x, colonPos.y + offsetShadow.y};
            DrawTextEx(BoldPixels, colon, colonShadowPos, fontSize, spacing, tintShadow);   // shadow
            DrawTextEx(BoldPixels, colon, colonPos, fontSize, spacing, tint);

            // Draw Minutes (Right Aligned to Colon)
            const char* minText = TextFormat("%02i", minutes);
            Vector2 minSize = MeasureTextEx(BoldPixels, minText, fontSize, spacing);
            Vector2 minPos = { colonPos.x - minSize.x - 4, yPos };  // 4 is padding
            Vector2 minShadowPos = {minPos.x + offsetShadow.x, minPos.y + offsetShadow.y};
            DrawTextEx(BoldPixels, minText, minShadowPos, fontSize, spacing, tintShadow);
            DrawTextEx(BoldPixels, minText, minPos, fontSize, spacing, tint);
            // Draw Seconds (Left Aligned to Colon)
            const char* secText = TextFormat("%02i", seconds);
            Vector2 secPos = { colonPos.x + colonSize.x + 4, yPos };
            Vector2 secShadowPos = {secPos.x + offsetShadow.x, secPos.y + offsetShadow.y};
            DrawTextEx(BoldPixels, secText, secShadowPos, fontSize, spacing, tintShadow);
            DrawTextEx(BoldPixels, secText, secPos, fontSize, spacing, tint);
        }
};