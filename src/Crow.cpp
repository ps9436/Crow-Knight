#include "Crow.hpp"

void Crow::Init(Vector2 startPos, int scale) {
    position = startPos;
    size = scale;

    // Load textures
    idleUNDEAD  = LoadTexture("assets/Charles-Idle(Undead)-Sheet.png");
    runUNDEAD   = LoadTexture("assets/Charles-Run(Undead)-Sheet.png");
    jumpUNDEAD  = LoadTexture("assets/Charles-Jump(Undead)-Sheet.png");
    idleALIVE   = LoadTexture("assets/Charles-Idle-Sheet.png");
    runALIVE    = LoadTexture("assets/Charles-Run-Sheet.png");
    jumpALIVE   = LoadTexture("assets/Charles-Jump-Sheet.png");
    attackSIDE  = LoadTexture("assets/Charles-SideAttack-Sheet.png");
    attackUP    = LoadTexture("assets/Charles-UpAttack-Sheet.png");
    attackDOWN  = LoadTexture("assets/Charles-DownAttack-Sheet.png");

    // Load Undead animations
    undeadIDLE.Init(idleUNDEAD, 6, 8.0f, size); undeadAnims[CharacterState::IDLE] = undeadIDLE;
    undeadRUN.Init(runUNDEAD, 3, 12.0f, size); undeadAnims[CharacterState::RUN] = undeadRUN;
    undeadJUMP.Init(jumpUNDEAD, 4, 16.0f, size, false); undeadAnims[CharacterState::JUMP] = undeadJUMP;

    // Load alive animations
    aliveIDLE.Init(idleALIVE, 2, 8.0f, size); aliveAnims[CharacterState::IDLE] = aliveIDLE;
    aliveRUN.Init(runALIVE, 3, 12.0f, size); aliveAnims[CharacterState::RUN] = aliveRUN;
    aliveJUMP.Init(jumpALIVE, 4, 16.0f, size, false); aliveAnims[CharacterState::JUMP] = aliveJUMP;
    // Load attack animations
    sideATTACK.Init(attackSIDE, 5, 20.0f, size, false); aliveAnims[CharacterState::ATTACK_SIDE] = sideATTACK;
    upATTACK.Init(attackUP, 5, 20.0f, size, false); aliveAnims[CharacterState::ATTACK_UP] = upATTACK;
    downATTACK.Init(attackDOWN, 5, 20.0f, size, false); aliveAnims[CharacterState::ATTACK_DOWN] = downATTACK;

    // Default
    animations = undeadAnims;
}

void Crow::Unload() {
    UnloadTexture(idleUNDEAD);
    UnloadTexture(runUNDEAD);
    UnloadTexture(jumpUNDEAD);
    UnloadTexture(idleALIVE);
    UnloadTexture(runALIVE);
    UnloadTexture(jumpALIVE);
    UnloadTexture(attackSIDE);
    UnloadTexture(attackUP);
    UnloadTexture(attackDOWN);
}

void Crow::Update(Input input) {
    CharacterState newState = currentState;

    // Determine newState from input
    if (input.attacked && currentForm == CrowForm::ALIVE) {
        // Check attack direction based on Input
        if (input.moveY < 0) {
            newState = CharacterState::ATTACK_UP;
        } 
        else if (input.moveY > 0) {
            newState = CharacterState::ATTACK_DOWN;
        } 
        else {
            newState = CharacterState::ATTACK_SIDE;
        }
    }
    // Jump input
    else if (input.jumped && onGround) {
        newState = CharacterState::JUMP;
        zVelocity = jumpPower;
        onGround = false;
    }
    // Aerial physics
    else if (!onGround) {
        newState = CharacterState::JUMP;
    }
    // Ground movement
    else if (input.moveX != 0 || input.moveY != 0) {
            newState = CharacterState::RUN;
    }
    // Default
    else {
        newState = CharacterState::IDLE;
    }

    // Try to switch states
    if (newState != currentState) {
        if (CanInterrupt(newState)) {
            currentState = newState;
            // Safety check before resetting
            if(animations.count(currentState) > 0) {
                animations[currentState].Reset();       // Reset animation before playing
            }
        }
    }

    // Form switching (stabbing)
    if (input.special) {
        SwitchForm();
    }

    // Update physics
    velocity.x = 0;
    velocity.y = 0;
    if (currentState == CharacterState::RUN || currentState == CharacterState::JUMP || IsAttacking()) {
        velocity.x += input.moveX * speed;
        velocity.y += input.moveY * speed;
        if (input.moveX > 0) faceRight = true;
        if (input.moveX < 0) faceRight = false;
    } else {
        velocity.x = 0;
        velocity.y = 0;
    }

    Character::Update(GetFrameTime());  // (pass in dt)
}

bool Crow::CanInterrupt(CharacterState nextState) {
    // If Idle or Running, we can do anything
    if (currentState == CharacterState::IDLE || currentState == CharacterState::RUN) return true;

    // If jumping, we can only attack or nothing
    if (currentState == CharacterState::JUMP) {
        // Allow switching to an Attack (Air Attack)
        if (nextState == CharacterState::ATTACK_SIDE || 
            nextState == CharacterState::ATTACK_UP || 
            nextState == CharacterState::ATTACK_DOWN) {
            return true;
        }
        // Switch to Idle/Run if on ground
        if (onGround) return true;
        return false; 
    }

    // Attack cancel rules
    if (IsAttacking()) {
        // Can cancel after active frames
        if (animations[currentState].currentFrame > 2 /* NOTE: "2" is the active frames */) return true;
        // Wait for animation to finish otherwise
        if (animations[currentState].IsFinished()) return true;
        return false;   // Locked in animation
    }
    return true;
}

void Crow::SwitchForm()
{
    if (currentForm == CrowForm::UNDEAD) {
        currentForm = CrowForm::ALIVE;
        animations = aliveAnims;        // Swap moveset
    } else { 
        currentForm = CrowForm::UNDEAD;
        animations = undeadAnims;
    }

    if (animations.find(currentState) == animations.end()) {
        currentState = CharacterState::IDLE;
    }
}
