#include "Crow.hpp"

void Crow::Init(Vector2 startPos, int scale) {
    position = startPos;
    size = scale;
    currentBlood = BLOOD;

    // Load textures
    idleUNDEAD  = LoadTexture("assets/crow/Charles-Idle(Undead)-Sheet.png");
    runUNDEAD   = LoadTexture("assets/crow/Charles-Run(Undead)-Sheet.png");
    jumpUNDEAD  = LoadTexture("assets/crow/Charles-Jump(Undead)-Sheet.png");
    idleALIVE   = LoadTexture("assets/crow/Charles-Idle-Sheet.png");
    runALIVE    = LoadTexture("assets/crow/Charles-Run-Sheet.png");
    jumpALIVE   = LoadTexture("assets/crow/Charles-Jump-Sheet.png");
    attackSIDE  = LoadTexture("assets/crow/Charles-SideAttack-Sheet.png");
    attackUP    = LoadTexture("assets/crow/Charles-UpAttack-Sheet.png");
    attackDOWN  = LoadTexture("assets/crow/Charles-DownAttack-Sheet.png");
    dashTexture = LoadTexture("assets/crow/Charles-Feather-Sheet.png");
    hurtTexture = LoadTexture("assets/crow/Charles-hurt-blood.png");
    switchTexture = LoadTexture("assets/crow/Charles-switch-blood.png");

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

    // Dash/blood animations
    dashAnim.Init(dashTexture, 10, 10.0f, size, false); aliveAnims[CharacterState::DASH] = dashAnim;
    hurtAnim.Init(hurtTexture, 7, 28, 2, false);
    switchAnim.Init(switchTexture, 6, 24, 1, false);

    shadow = LoadTexture("assets/Shadow.png");
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
    UnloadTexture(shadow);
    UnloadTexture(dashTexture);
    UnloadTexture(hurtTexture);
    UnloadTexture(switchTexture);
}

void Crow::Update(Input input, float dt) {
    dashAnim.Update(dt);    // Update dash animation independent of dash
    if (isHurt) {
        hurtAnim.Update(dt);
        if (hurtAnim.IsFinished()) {
            hurtAnim.Reset();
            isHurt = false;
        }
    }
    if (switched) {
        switchAnim.Update(dt);
        if (switchAnim.IsFinished()) {
            switchAnim.Reset();
            switched = false;
        }
    }
    
    if (currentForm == CrowForm::UNDEAD) Heal();

    if (currentForm == CrowForm::ALIVE && currentState != CharacterState::DEATH) {
        currentBlood -= bloodDrainRate * dt;
        if (currentBlood <= 0.0f) {
            currentBlood = 0.0f;
            currentState = CharacterState::DEATH;
        }
    }
    if (currentState == CharacterState::DASH) {
        // Dash cancel with attack
        if (input.attacked && currentForm == CrowForm::ALIVE) {
            
            // Switch to an attack state
            if (input.moveY < 0) currentState = CharacterState::ATTACK_UP;
            else if (input.moveY > 0) currentState = CharacterState::ATTACK_DOWN;
            else currentState = CharacterState::ATTACK_SIDE;
            // Reset physics (probably no need)
            velocity = {0,0}; 
            zVelocity = 0;
            // Reset the attack animation
            if (animations.count(currentState) > 0) animations[currentState].Reset();
            // Next frame, currentState is ATTACK, so it skips this block entirely.
            return;     // Skip Dash math
        }

        Dash(input, dt);
        return; // If dashed, skip standard physics
    }

    // Determine newState from input
    CharacterState newState = currentState;
    // Dash handler (dash only when alive - first priority)
    if (input.dashed && currentState != CharacterState::DASH && currentForm == CrowForm::ALIVE) {
        newState = CharacterState::DASH;

        currentBlood -= dashCost;

        // Setup timers/positions
        dashTimer = 0.0f;
        dashStartPos = position;
        // If moving, dash that way. If idle, dash where facing
        float dirX = 0.0f;
        float dirY = 0.0f;
        if (input.moveX != 0 || input.moveY != 0) {
            dirX = input.moveX;
            dirY = input.moveY;
        } else dirX = faceRight ? 0.0f : -0.0f;     // 0:0 if dash in place is enabled, otherwise 1:1
        // Distance = Speed * Multiplier
        float dist = speed * 0.5f;
        dashTargetPos = { position.x + (dirX * dist), position.y + (dirY * dist)};

        // Prep dash animation
        dashEffectPos = position;
        dashAnim.Reset();
    }
    // Attack input
    else if (input.attacked && currentForm == CrowForm::ALIVE) {
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
    // If we are attacking again
    if (newState == CharacterState::ATTACK_DOWN || 
        newState == CharacterState::ATTACK_UP || 
        newState == CharacterState::ATTACK_SIDE) {
            if (CanInterrupt(newState)) {
                currentState = newState;
                if(animations.count(currentState) > 0) {
                    animations[currentState].Reset();
                }
            }
        }
    else if (newState != currentState) {
        if (CanInterrupt(newState)) {
            currentState = newState;
            // Safety check before resetting
            if(animations.count(currentState) > 0) {
                animations[currentState].Reset();       // Reset animation before playing
            }
        }
    }

    // Form switching (stabbing)`
    if (input.special) {
        Special();
        if (currentForm == CrowForm::UNDEAD) {
            switched = true;
        }
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

    Character::Update(dt);  // (pass in dt)
}

void Crow::Draw() {
    if (!dashAnim.IsFinished()) dashAnim.Draw(dashEffectPos);
    if (isHurt) hurtAnim.Draw(position, faceRight);
    if (switched) switchAnim.Draw({position.x, position.y - z}, faceRight);
    Character::Draw();
}

void Crow::LifeSteal(float amount) {
    lifeStolen += amount * lifeSteal;
    if (lifeStolen >= BLOOD) lifeStolen = BLOOD;
}

void Crow::Heal() {
    if (currentBlood >= BLOOD) return;
    if (lifeStolen >= BLOOD) {
        currentBlood += lifeStolen;     // If lifesteal full then heal instantly
        if (currentBlood >= BLOOD) currentBlood = BLOOD;
        lifeStolen = 0.0f;
    }
    else {
        currentBlood += lifeStolen * lifeSteal * GetFrameTime();    // Otherwise heal over time
        lifeStolen -= lifeStolen * lifeSteal * GetFrameTime();
        if (currentBlood >= BLOOD) currentBlood = BLOOD;
        if (lifeStolen <= 0) lifeStolen = 0.0f;
    }
}

void Crow::TakeDamage(float dps) {
    if (currentState == CharacterState::DEATH) return;
    if (dps > 50.0f) dps = 50.0f; // Max take 50 damage per second
    currentBlood -= dps * GetFrameTime();
    isHurt = true;
    if (currentBlood < 0.0f) {
        currentBlood = 0.0f;
        currentState = CharacterState::DEATH;
    }
}

float Crow::GetBloodPercent() {
    return currentBlood / BLOOD;
}

float Crow::GetLifeStealPercent() {
    return lifeStolen / BLOOD;
}

bool Crow::IsAlive() const {return currentBlood > 0.0f; }

bool Crow::CanInterrupt(CharacterState nextState) {
    // If Idle or Running, we can do anything
    if (currentState == CharacterState::IDLE || 
        currentState == CharacterState::RUN || 
        currentState == CharacterState::HURT) return true;
    
    // If jumping, we can only attack or nothing
    if (currentState == CharacterState::JUMP) {
        // Allow air attack and dash
        if (nextState == CharacterState::ATTACK_SIDE || 
            nextState == CharacterState::ATTACK_UP || 
            nextState == CharacterState::ATTACK_DOWN ||
            nextState == CharacterState::DASH) {
            return true;
        }
        // Switch to Idle/Run if on ground
        if (onGround) return true;
        return false; 
    }

    // Attack cancel rules
    if (IsAttacking()) {
        // Attack cancel with dashs
        if (nextState == CharacterState::DASH) return true;
        // Can cancel after active frames
        if ((nextState == CharacterState::ATTACK_DOWN || 
            nextState == CharacterState::ATTACK_UP || 
            nextState == CharacterState::ATTACK_SIDE) &&
            animations[currentState].currentFrame > attackSpeed) return true;
        // Wait for animation to finish otherwise
        if (animations[currentState].IsFinished()) return true;
        return false;   // Locked in animation
    }
    return true;
}

void Crow::Special()
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

float Crow::EaseInOutQuad(float t) {
    if (t < 0.5f) {
        return 2.0f * t * t;    // Accelerate
    } else {
        return -1.0f + (4.0f - 2.0f * t) * t;   // Decelerate
    }
}

void Crow::Dash(Input input, float dt) {
    // float dt = GetFrameTime();
    dashTimer += dt;

    // Normalize time
    float time = dashTimer / dashDuration;

    if (time >= 1.0f) {     // onGround may not be updated
        // End Dash based on input
        if (z > 0.0f) currentState = CharacterState::JUMP;
        else if (input.moveX != 0 || input.moveY != 0) currentState = CharacterState::RUN;
        else currentState = CharacterState::IDLE;

        velocity = {0,0};
        zVelocity = 0;      // Kill gravity for a moment
    } else {
        // Interpolate position
        float progress = EaseInOutQuad(time);

        // Lerp formula: postion = Start + (End - Start) * Progress
        position.x = dashStartPos.x + (dashTargetPos.x - dashStartPos.x) * progress;
        position.y = dashStartPos.y + (dashTargetPos.y - dashStartPos.y) * progress;
    }
}
