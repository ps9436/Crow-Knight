#pragma once
#include <raylib.h>

struct Animation {
    Texture2D texture;          // The actual sprite
    int totalFrames;            // Total number of frames within the sprite
    int currentFrame = 0;       // The current frame on the sprite
    float frameSpeed;           // FPS
    float timer = 0.0f;         // Count up difference in time (dt) until next frame
    float scale = 1.0f;         // Scale of texture
    bool loop = true;           // Should the animation loop or not

    // Initialize the sprite (contructor)
    void Init(Texture2D tex, int frames, float speed, float spriteScale = 1.0f, bool shouldLoop = true) {
        texture = tex;
        totalFrames = frames;
        frameSpeed = speed;
        scale = spriteScale;
        loop = shouldLoop;
    }
   
    void Update(float dt) {
        timer += dt;        // dt is the time (s) that passed since the last frame
        if (timer >= (1.0f / frameSpeed)) {
            timer = 0.0f;
            if (loop) {
                currentFrame++;
                if (currentFrame >= totalFrames) {
                    currentFrame = 0;
                }
            } else {
                if (currentFrame < totalFrames - 1) currentFrame++;
                // else: stay on the last frame
            }
        }
    }

    void Draw(Vector2 position, bool flipH = false, float rotation = 0.0f, Color tint = WHITE) {
        float width = (float)texture.width / totalFrames;
        float height = (float)texture.height;

        Rectangle source = {
            currentFrame * width,
            0,
            flipH ? -width : width,     // Flip texture horizontally if needed
            height
        };
        Rectangle dest = { position.x, position.y, width * scale, height * scale};

        // Instead of {0,0} to {dest.width/2,dest.height/2} for center sprite drawing
        DrawTexturePro(texture, source, dest, {dest.width/2,dest.height/2}, rotation, tint);      
    }

    void DrawScaled(Vector2 position, float scaleMult, bool flipH = false, Color tint = WHITE) {
        float width = (float)texture.width / totalFrames;
        float height = (float)texture.height;

        Rectangle source = {
            currentFrame * width,
            0,
            flipH ? -width : width,
            height
        };
        
        // Multiply the base scale by the specific scale multiplier
        float finalScale = scale * scaleMult;

        Rectangle dest = { position.x, position.y, width * finalScale, height * finalScale};
        DrawTexturePro(texture, source, dest, {dest.width/2, dest.height/2}, 0.0f, tint);      
    }

    // For character state switching
    // Character plays the beginning of their anim (not in the middle)
    void Reset() {
        currentFrame = 0;
        timer = 0.0f;
    }

    // Check if animation (attack) ended
    bool IsFinished() const {
        return (!loop) && (currentFrame >= totalFrames - 1);
    }
};