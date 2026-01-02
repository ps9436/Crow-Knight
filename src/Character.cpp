#include "Character.hpp"

void Character::Update(float dt) {

    // X/Y movement
    position.x += velocity.x * dt;
    position.y += velocity.y * dt;

    // Z-axis
    zVelocity -= gravity * dt;
    z += zVelocity * dt;

    // Ground collision
    if (z <= 0.0f) {
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

void Character::DrawShadow() {
    // If no animation is loaded, we can't draw a shadow
    if (animations.count(currentState) == 0) return;

    // Dimensions of current sprite
    Animation& anim = animations[currentState];

    // Adjust shadow postion
    Vector2 shadowPos = {
        position.x,
        position.y
    };

    // How much shadow shrinks as we jump
    float jumpScale = 1.0f - (z / 500.0f); 
    // if (jumpScale < 0.2f) jumpScale = 0.2f;
    float finalScale = jumpScale * anim.scale;

    // Adjust the shadow if character flipped
    float sourceWidth = (float)shadow.width;
    if (faceRight) sourceWidth *= -1;

    // Draw
    Rectangle source = { 0, 0, sourceWidth, (float)shadow.height };
    Rectangle dest = {
        shadowPos.x,      // Center X on feet
        shadowPos.y,      // Center Y on feet
        (float)shadow.width * finalScale,  // Width
        (float)shadow.height * finalScale  // Height
    };
    Vector2 center { dest.width / 2, dest.height / 2 };
    DrawTexturePro(shadow, source, dest, center, 0.0f, WHITE);
}

void Character::Draw() {
    if (animations.count(currentState) > 0) {
        if (currentState != CharacterState::DEATH) DrawShadow();
        Vector2 drawPos = { position.x, position.y - z};
        animations[currentState].Draw(drawPos, faceRight, tint);
    }
}

void Character::AddAnimation(CharacterState state, Animation anim) {
    animations[state] = anim;
}
