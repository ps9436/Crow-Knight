#include "Animation.hpp"
#include <raylib.h>
#include "Input.hpp"

void Animation::Init(const std::string& filename, int frames, float speed, Vector2 pos, int spriteScale, bool repeat) {
    texture = LoadTexture(filename.c_str());
    // SetTextureFilter(texture, TEXTURE_FILTER_POINT); // Prevent blurry scaling

    totalFrames = frames;
    currentFrame = 0;
    frameTime = 0.0f;
    frameSpeed = speed;
    position = pos;
    scale = spriteScale;
    loop = repeat;

    frameRec = { 0.0f, 0.0f, ((float)texture.width / totalFrames), ((float)texture.height) };
}

void Animation::Update() {
    frameTime += GetFrameTime();
    if (frameTime >= (1.0f / frameSpeed)) {
        frameTime = 0.0f;

        if (loop) {
            currentFrame++;
            if (currentFrame >= totalFrames) {
            currentFrame = 0;
            }
        } else {
            if (currentFrame < totalFrames - 1) {
                currentFrame++;
            }
            // else: stay on the last frame
        }
        frameRec.x = (float)currentFrame * frameRec.width;
    }
}

void Animation::Draw(bool flipX) {
    Rectangle source = frameRec;                // Texture space
    Rectangle dest = {position.x, position.y,   // Screen space
        frameRec.width * scale, frameRec.height * scale};
    
    Vector2 center = { dest.width / 2, dest.height / 2};
    if (flipX) source.width = -source.width;

    DrawTexturePro(texture, source, dest, center, 0.0f, WHITE);
}

void Animation::SetPosition(Vector2 pos) {
    position = pos;
}

void Animation::Reset() {
    currentFrame = 0;
    frameTime = 0.0f;
    frameRec.x = 0.0f;
}

void Animation::Unload() {
    UnloadTexture(texture);
}