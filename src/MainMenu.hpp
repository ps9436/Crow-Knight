#pragma once
#include <raylib.h>

class MainMenu {
    private:
        // UI
        Font BoldPixels;
        int screenWidth;
        int screenHeight;
        Rectangle buttonRect;

        // Animation
        const int TOTAL_FLASHES = 8; 
        const float FLASH_SPEED = 0.08f;
        float flashTimer = 0.0f;
        int flashCount = 0;
        bool isWhiteFlash = false;
        bool isHovered;
        bool pressedPlay = false;
         

        // Crow
        Texture2D idleUNDEAD;
        Texture2D idleALIVE;
        Animation undeadIDLE;
        Animation aliveIDLE;
        int size = 10;
        
    public:
        void Init(int sWidth, int sHeight) {
            BoldPixels = LoadFontEx("assets/BoldPixels.ttf", 16, 0, 0);
            SetTextureFilter(BoldPixels.texture, TEXTURE_FILTER_POINT);
            screenWidth = sWidth;
            screenHeight = sHeight;
            
            // Button Dimensions
            float w = 300;
            float h = 100;
            // Center button in lower half
            buttonRect = { (float)(screenWidth - w)/2, (float)(screenHeight - h)/2 + 200, w, h };
            isHovered = false;

            idleUNDEAD = LoadTexture("assets/crow/(outline)Dead-Idle-Sheet.png");
            idleALIVE = LoadTexture("assets/crow/(outline)Idle-Sheet.png");
            undeadIDLE.Init(idleUNDEAD, 6, 4.0f, size);
            aliveIDLE.Init(idleALIVE, 2, 8.0f, size);
        }

        // Returns true if Play button is clicked
        bool Update() {
            Vector2 mouse = GetMousePosition();
            
            aliveIDLE.Update(GetFrameTime());
            
            if (CheckCollisionPointRec(mouse, buttonRect)) {
                isHovered = true;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    pressedPlay = true;
                    aliveIDLE.Init(idleALIVE, 2, 12.0f, size);
                }
            } else {
                isHovered = false;
                undeadIDLE.Update(GetFrameTime());
            }
            if (pressedPlay) {
                // Flash Animation Logic
                flashTimer += GetFrameTime();
                if (flashTimer >= FLASH_SPEED) {
                    flashTimer = 0.0f;
                    isWhiteFlash = !isWhiteFlash; // Toggle white/normal
                    flashCount++;
                    // End Animation
                    if (flashCount >= TOTAL_FLASHES) {
                        return true;
                    }
                }
            }
            return false;
        }

        void Draw() {
            // Draw Background
            DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

            // TITLE
            const char* title = "CROW KNIGHT";
            float fontSize = 150;
            float spacing = 5;
            
            Vector2 textSize = MeasureTextEx(BoldPixels, title, fontSize, spacing);
            Vector2 textPos = { (screenWidth - textSize.x)/2 + 20, 150 };
            
            // Title Shadow
            DrawTextEx(BoldPixels, title, {textPos.x-6, textPos.y+6}, fontSize, spacing, MAROON);
            // Title Main (Yellow/Red flicker style or just White)
            DrawTextEx(BoldPixels, title, textPos, fontSize, spacing, RAYWHITE);

            // PLAY BUTTON
            float yOffset = isHovered ? -10 : 0;
            
            Rectangle shadowRect = { buttonRect.x - 6, buttonRect.y + 6, buttonRect.width, buttonRect.height };
            Rectangle drawRect = { buttonRect.x, buttonRect.y + yOffset, buttonRect.width, buttonRect.height };

            // Shadow
            DrawRectangleRec(shadowRect, MAROON);
            
            // Button Body
            Color btnColor = isWhiteFlash ? BLACK : RAYWHITE; 
            DrawRectangleRec(drawRect, btnColor);

            // Button Text
            const char* btnText = "PLAY";
            float btnFontSize = 70;
            Vector2 btnSize = MeasureTextEx(BoldPixels, btnText, btnFontSize, 2);
            Vector2 btnPos = { 
                drawRect.x + (drawRect.width - btnSize.x)/2, 
                drawRect.y + (drawRect.height - btnSize.y)/2 
            };
            
            Color shadowColor = isWhiteFlash ? MAROON : BLACK; 
            Color textColor = isWhiteFlash ? RAYWHITE : MAROON; 
            DrawTextEx(BoldPixels, btnText, {btnPos.x-4, btnPos.y+4}, btnFontSize, 2, shadowColor);
            DrawTextEx(BoldPixels, btnText, btnPos, btnFontSize, 2, textColor);

            Vector2 drawPos = {(screenWidth/2) + 40, screenHeight/2};
            if (pressedPlay) aliveIDLE.Draw(drawPos);
            else {
                if (isHovered) aliveIDLE.Draw(drawPos);
                else           undeadIDLE.Draw(drawPos);
            }
        }
};