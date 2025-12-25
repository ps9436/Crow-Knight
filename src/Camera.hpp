#pragma once
#include <raylib.h>

enum class CameraMode {
    FOLLOW,
    LOCKED
};

class Camera {
    public:
        Camera2D raylibCam = { 0 };
        CameraMode currentMode = CameraMode::FOLLOW;

        // Settings
        float zoom = 1.0f;
        float smoothSpeed = 0.1f;
        
        Vector2 lockedTarget = {0,0};   // Center of screen

        void Init(int screenWidth, int screenHeight) {
            raylibCam.target = { 0, 0 };
            
            raylibCam.offset = { (float)screenWidth/2.0f, (float)screenHeight/2.0f};
            raylibCam.rotation = 0.0f;
            raylibCam.zoom = zoom;
        }

        void Update(Vector2 playerPos) {
            Vector2 targetPos;

            // Determine target
            if (currentMode == CameraMode::FOLLOW) {
                targetPos = playerPos;
            } else {
                targetPos = lockedTarget;
            }

            // Move camera smoothly
            raylibCam.target.x += (targetPos.x - raylibCam.target.x) * smoothSpeed;
            raylibCam.target.y += (targetPos.y - raylibCam.target.y) * smoothSpeed;
        }

        // Helpers to switch camera modes
        void LockTo(Vector2 position) {
            currentMode = CameraMode::LOCKED;
            lockedTarget = position;
        }
        void FollowPlayer() {
            currentMode = CameraMode::FOLLOW;
        }
};