# Crow Knight
A fast-paced, 2.5D action roguelike built from scratch using C++ and Raylib.

# Features
Custom Physics Engine: Vector-based movement with friction, bounce damping, and z-axis jumping.

Spatial Grid System: Optimized collision detection handling 2000+ entities at 60 FPS.

Player AI: State-machine behaviors including idle, running, jumping, dashing, 3 different attack types with specific attack-cancel logic.

Mob AI: State-machine behaviors for enemies, including chasing, flocking, attacking, shooting, and other special attacks!

Progression: XP orb system with orbital physics and a randomized card-based upgrade menu.

# Tech Stack
Language: C++

Library: Raylib (Rendering, Audio, Input)

Architecture: Component-based entity system

# How to Build
Ensure you have a C++ compiler and Raylib installed.

Clone the repository.

Compile main.cpp linking against Raylib (e.g., -lraylib).

Compile and run!
