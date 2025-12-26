#pragma once
#include <raylib.h>

enum class CameraState {
    FOLLOW,
    LOCKED
};

class CameraManager {
    public:
        Camera2D raylibCam = { 0 };
        CameraState currentMode = CameraState::FOLLOW;

        // Settings
        float zoom = 1.0f;
        float smoothSpeed = 0.2f;
        
        Vector2 lockedTarget = {0,0};       // Center of screen
        float lookAheadDistance = 50.0f;   // How many pixels to look forward
        float currentLookAhead = 0.0f;      // Internal smoother

        void Init(int screenWidth, int screenHeight, Vector2 startPos) {            
            // Offset is the center point of monitor
            raylibCam.offset = { (float)screenWidth/2, (float)screenHeight/2};
            raylibCam.target = startPos;
            raylibCam.rotation = 0.0f;
            raylibCam.zoom = zoom;
        }

        void Update(Vector2 playerPos, bool faceRight) {
            Vector2 targetPos;

            // Determine target
            if (currentMode == CameraState::FOLLOW) {
                float direction = faceRight ? 1.0f : -1.0f;
                float targetOffset = lookAheadDistance * direction;
                // Smoothly get to target offset
                currentLookAhead += (targetOffset - currentLookAhead) * 0.05f;
                // Set final target
                targetPos.x = playerPos.x + currentLookAhead;
                targetPos.y = playerPos.y;
            } else {
                targetPos = lockedTarget;
            }

            // Move camera smoothly
            raylibCam.target.x += (targetPos.x - raylibCam.target.x) * smoothSpeed;
            raylibCam.target.y += (targetPos.y - raylibCam.target.y) * smoothSpeed;
        }

        // Helpers to switch camera modes
        void SwitchCamera(Vector2 pos) {
            if (currentMode == CameraState::LOCKED) FollowPlayer();
            else LockTo(pos);
        }
        void LockTo(Vector2 position) {
            currentMode = CameraState::LOCKED;
            lockedTarget = position;
        }
        void FollowPlayer() {
            currentMode = CameraState::FOLLOW;
        }
};