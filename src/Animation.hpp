#pragma once
#include <raylib.h>
#include <string>

class Animation {
    private:
        Texture2D texture;      // The actual sprite
        int totalFrames;        // Total number of frames within the sprite
        int currentFrame;       // The current frame on the sprite
        float frameTime;        // Amount of time for each frame
        float frameSpeed;       // FPS
        bool loop;              // Should the animation loop or not
        Vector2 position;       // Postion of the sprite
        Rectangle frameRec;     // Rectangle going over each individual frame
        int scale{1};

    public:
        // Method decelerations
        void Init(const std::string& filename, int frames, float speed, Vector2 pos, int spriteScale = 1, bool repeated = true);     // Initialize the sprite (contruc)
        void Update();              // Update the frame of the sprite
        void Draw(bool flipX);      // Draw the texture flipped or not (DrawTexturePro)
        void SetPosition(Vector2 pos);
        void Reset();
        bool IsFinished() const;
        void Unload();
};