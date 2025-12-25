#include "Character.hpp"

void Character::Update(float dt) {

    // X/Y movement
    position.x = velocity.x * dt;
    position.y = velocity.y * dt;

    // Z-axis
    zVelocity -= gravity * dt;
    z += zVelocity * dt;

    // Ground collision
    if (z >= 0.0f) {
        z = 0.0f;
        zVelocity = 0.0f;
        onGround = true;
    } else {
        onGround = false;
    }

    // Animation - only update if current state is in map
    if (animations.count(currentState) > 0) {
        animations[currentState].Update(dt);
    }
}

void Character::Draw() {
    if (animations.count(currentState) > 0) {
        Vector2 drawPos = { position.x, position.y - z};
        animations[currentState].Draw(drawPos, faceRight);
    }
}

void Character::AddAnimation(CharacterState state, Animation anim) {
    animations[state] = anim;
}
