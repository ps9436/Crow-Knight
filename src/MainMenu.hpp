#pragma once
#include <raylib.h>

class MainMenu {
    private:
        Font BoldPixels;
        int screenWidth;
        int screenHeight;
        Rectangle buttonRect;
        bool isHovered;
        
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
            buttonRect = { (float)(screenWidth - w)/2, (float)(screenHeight - h)/2 + 100, w, h };
            isHovered = false;
        }

        // Returns true if Play button is clicked
        bool Update() {
            Vector2 mouse = GetMousePosition();
            
            if (CheckCollisionPointRec(mouse, buttonRect)) {
                isHovered = true;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    return true;
                }
            } else {
                isHovered = false;
            }
            return false;
        }

        void Draw() {
            // Draw Background
            DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);

            // TITLE
            const char* title = "CROW KNIGHT";
            float fontSize = 100;
            float spacing = 5;
            
            Vector2 textSize = MeasureTextEx(BoldPixels, title, fontSize, spacing);
            Vector2 textPos = { (screenWidth - textSize.x)/2, (screenHeight/2) - 150 };
            
            // Title Shadow
            DrawTextEx(BoldPixels, title, {textPos.x-6, textPos.y+6}, fontSize, spacing, RAYWHITE);
            // Title Main (Yellow/Red flicker style or just White)
            DrawTextEx(BoldPixels, title, textPos, fontSize, spacing, MAROON);

            // PLAY BUTTON
            float yOffset = isHovered ? -10 : 0;
            
            Rectangle shadowRect = { buttonRect.x - 6, buttonRect.y + 6, buttonRect.width, buttonRect.height };
            Rectangle drawRect = { buttonRect.x, buttonRect.y + yOffset, buttonRect.width, buttonRect.height };

            // Shadow
            DrawRectangleRec(shadowRect, RAYWHITE);
            
            // Button Body
            Color btnColor = isHovered ? BLACK : BLACK; 
            DrawRectangleRec(drawRect, btnColor);

            // Button Text
            const char* btnText = "PLAY";
            float btnFontSize = 70;
            Vector2 btnSize = MeasureTextEx(BoldPixels, btnText, btnFontSize, 2);
            Vector2 btnPos = { 
                drawRect.x + (drawRect.width - btnSize.x)/2, 
                drawRect.y + (drawRect.height - btnSize.y)/2 
            };
            
            DrawTextEx(BoldPixels, btnText, {btnPos.x-3, btnPos.y+3}, btnFontSize, 2, RAYWHITE);
            DrawTextEx(BoldPixels, btnText, btnPos, btnFontSize, 2, RED);
        }
};