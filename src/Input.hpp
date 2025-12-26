#pragma once
#include <raylib.h>

struct Input {
    float moveX;
    float moveY;
    bool jumped;
    bool attacked;
    bool special;
    bool dashed;
    bool cameraLocked;
};
