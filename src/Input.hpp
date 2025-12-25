#pragma once
#include <raylib.h>

struct Input {
    float moveX;
    float moveY;
    bool special;
    bool jumped;
    bool attacked;
};
