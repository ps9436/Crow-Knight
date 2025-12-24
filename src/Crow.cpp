#include "Crow.hpp"
#include <string.h>
#include <iostream>

void Crow::Init(Vector2 startPos, int spriteScale) {
    position = startPos;
    form = CrowForm::Undead;
    state = CrowState::Idle;

    // Animations
    undeadIdle.Init("assets/Charles-Idle(Undead)-Sheet.png", 6, 8.0f, position, spriteScale);
    undeadRun.Init("assets/Charles-Run(Undead)-Sheet.png", 3, 12.0f, position, spriteScale);
    undeadJump.Init("assets/Charles-Jump(Undead)-Sheet.png", 4, 16.0f, position, spriteScale, false);
    aliveIdle.Init("assets/Charles-Idle-Sheet.png", 2, 8.0f, position, spriteScale);
    aliveRun.Init("assets/Charles-Run-Sheet.png", 3, 12.0f, position, spriteScale);
    aliveJump.Init("assets/Charles-Jump-Sheet.png", 4, 16.0f, position, spriteScale, false);
    sideAttack.Init("assets/Charles-SideAttack-Sheet.png", 5, 20.0f, position, spriteScale, false);
}

void Crow::Update(Input input) {
    float dt = GetFrameTime();

    if (input.stabbed) {
        Stab();
    }

    position.x += input.moveX * moveSpeed * dt;
    position.y += input.moveY * moveSpeed * dt;

    if (input.jumped && onGround) {
        onGround = false;
        zVelocity = jumpPower;
        SetState(CrowState::Jump);
        GetAnimation().Reset();
    }

    if (!onGround) {
        SetState(CrowState::Jump);
    } else {
        if ((input.moveX || input.moveY) != 0) {
            SetState(CrowState::Run);
            if (input.moveX > 0) faceRight = true;
            else if (input.moveX < 0) faceRight = false;
        } else if (input.attacked) {
            SetState(CrowState::Attack);
            GetAnimation().Reset();
        } else {
            SetState(CrowState::Idle);
        }
    }
    

    // We're in the air
    if (!onGround) {
        zVelocity -= gravity * dt;
        z += zVelocity * dt;

        if (z <= 0.0f) {
            z = 0.0f;
            zVelocity = 0.0f;
            onGround = true;
        }
    }

    Vector2 drawPos = position;
    drawPos.y -= z;

    Animation& anim = GetAnimation();
    anim.SetPosition(drawPos);
    anim.Update();
}

void Crow::Draw() {

    // Vector2 center = { dest.width / 2, dest.height / 2};
    // Vector2 shadow = { position.x - 15, position.y + 33 };
    // if (flipX) {
    //     source.width = -source.width;
    //     Vector2 flipshadow = { position.x + 15, position.y + 33 };
    // }
    // DrawEllipse(shadow.x, shadow.y, 20, 4, Fade(BLACK, 0.25f));

    GetAnimation().Draw(faceRight);
}

void Crow::SetPosition(Vector2 pos) {
    position = pos;
    // Sync up animation positions
    // GetAnimation().SetPosition(pos);
}

Animation& Crow::GetAnimation() {
    if (form == CrowForm::Undead) {
        switch (state) {
        case CrowState::Idle:   return undeadIdle;
        case CrowState::Run:    return undeadRun;
        case CrowState::Jump:   return undeadJump;
        default:                return undeadIdle;
        }
    } 
    else if (form == CrowForm::Alive) {
        switch (state) {
        case CrowState::Idle:   return aliveIdle;
        case CrowState::Run:    return aliveRun;
        case CrowState::Jump:   return aliveJump;
        case CrowState::Attack: return sideAttack;
        }
    }
    // return undeadIdle;
}

void Crow::Unload() {
    undeadIdle.Unload();
    undeadRun.Unload();
    aliveIdle.Unload();
    aliveRun.Unload();
    sideAttack.Unload();
}

void Crow::Stab() {
    if (form == CrowForm::Undead) form = CrowForm::Alive;
    else                          form = CrowForm::Undead;
}

void Crow::SetState(CrowState newState) {
   if (state == newState) return;
   state = newState;
}
