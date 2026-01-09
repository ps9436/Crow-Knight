#pragma once
#include <raylib.h>

enum BorderType {
    TOP_LEFT, TOP_RIGHT, BOTTOM_LEFT, BOTTOM_RIGHT,
    HORIZONTAL, VERTICAL, EMPTY
};

class BorderMap {
private:
    Texture2D texture;
    const int SCALE = 4;

    Rectangle GetSourceRect(BorderType type) {
        switch (type) {
            case HORIZONTAL:   return { 0, 0, 16, 20 };
            case VERTICAL:     return { 16, 0, 16, 15 };
            case BOTTOM_LEFT:  return { 32, 0, 16, 21 };
            case BOTTOM_RIGHT: return { 48, 0, 16, 21 };
            case TOP_LEFT:     return { 64, 0, 16, 21 };
            case TOP_RIGHT:    return { 80, 0, 16, 21 };
            default: return { 0,0,0,0 };
        }
    }

    // Draws a single tile at a specific grid coordinate
    void DrawTile(int x, int y, BorderType type, int cellSize) {
        if (type == EMPTY) return;

        Rectangle src = GetSourceRect(type);
        
        // Calculate Position
        float drawX = x * cellSize;
        float drawY = y * cellSize;

        // Draw (Scaled by 4)
        DrawTexturePro(texture, src, 
            { drawX, drawY, src.width * SCALE, src.height * SCALE }, 
            {0,0}, 0.0f, WHITE);
    }

public:
    void Init() {
        texture = LoadTexture("assets/map/border-map.png");
    }

    void Unload() {
        UnloadTexture(texture);
    }

    void Draw(int tilesWide, int tilesHigh, int cellSize) {
        
        // Skip corners to not get drawn underneath walls
        for (int x = 0; x < tilesWide; x++) {
            for (int y = 0; y < tilesHigh; y++) {
                
                BorderType type = EMPTY;

                // Only check for Walls here
                if (y == 0 || y == tilesHigh-1) type = HORIZONTAL;
                else if (x == 0 || x == tilesWide-1) type = VERTICAL;
                
                // CRITICAL: If this coordinate is actually a corner, SKIP IT.
                bool isCorner = (x == 0 && y == 0) || 
                                (x == tilesWide-1 && y == 0) || 
                                (x == 0 && y == tilesHigh-1) || 
                                (x == tilesWide-1 && y == tilesHigh-1);

                if (!isCorner && type != EMPTY) {
                    DrawTile(x, y, type, cellSize);
                }
            }
        }

        // Draw corners
        DrawTile(0, 0, TOP_LEFT, cellSize);
        DrawTile(tilesWide - 1, 0, TOP_RIGHT, cellSize);
        DrawTile(0, tilesHigh - 1, BOTTOM_LEFT, cellSize);
        DrawTile(tilesWide - 1, tilesHigh - 1, BOTTOM_RIGHT, cellSize);
    }
};