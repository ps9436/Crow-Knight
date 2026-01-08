#pragma once
#include <raylib.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <random>
#include "Crow.hpp"

enum class UpgradeType {
    BLOOD,      // Red
    MOVEMENT,   // Blue
    LEGENDARY   // Yellow
};

struct UpgradeCard {
    std::string title;
    std::string description;
    UpgradeType type;
    int weight;     // Higher weight = more common
    Rectangle rect;
    bool isHovered;

    std::function<void(Crow&)> applyEffect;
};

class Upgrade {
    private:
        Font BoldPixels;
        int screenWidth;
        int screenHeight;
        std::vector<UpgradeCard> choosingCards; // 3 selected cards 
        std::vector<UpgradeCard> upgradePool;   // All possible cards

        const int CARD_WIDTH = 300;
        const int CARD_HEIGHT = 200;
        const int GAP = 50;

        // Animation State
        enum class MenuState {
            WAITING_FOR_INPUT,
            ANIMATING_SELECTION
        };
        
        MenuState currentState = MenuState::WAITING_FOR_INPUT;
        int selectedIndex = -1;
        
        // Flashing logic
        float flashTimer = 0.0f;
        int flashCount = 0;
        bool isWhiteFlash = false;
        
        const int TOTAL_FLASHES = 12; 
        const float FLASH_SPEED = 0.08f; 

    public:
        void Init(int sWidth, int sHeight) {
            BoldPixels = LoadFontEx("assets/BoldPixels.ttf", 16, 0, 0);
            SetTextureFilter(BoldPixels.texture, TEXTURE_FILTER_POINT);
            screenWidth = sWidth;
            screenHeight = sHeight;

            choosingCards.clear();
            upgradePool.clear();

            // All Upgrades
            // Speed Up (Common)
            UpgradeCard card1;
            card1.title = "Speed Up";
            card1.description = "Increase Speed and\nDash Distance\nby 20%";
            card1.type = UpgradeType::MOVEMENT;
            card1.weight = 5; 
            card1.applyEffect = [](Crow& c) { c.speed *= 1.2f; };
            upgradePool.push_back(card1);

            // Dash Up (Common)
            UpgradeCard card2;
            card2.title = "Fast Dash";
            card2.description = "Increase Dash Speed\nby 20%";
            card2.type = UpgradeType::MOVEMENT;
            card2.weight = 5;
            card2.applyEffect = [](Crow& c) { c.dashDuration *= 0.8f; };
            upgradePool.push_back(card2);

            // Jump Up (Common)
            UpgradeCard card3;
            card3.title = "Jump Up";
            card3.description = "Increase Jump Power\nby 20%";
            card3.type = UpgradeType::MOVEMENT;
            card3.weight = 5;
            card3.applyEffect = [](Crow& c) { c.jumpPower *= 1.2f; };
            upgradePool.push_back(card3);

            // Blood Up (Common)
            UpgradeCard card4;
            card4.title = "Blood Up";
            card4.description = "Increase Health\nby 20%";
            card4.type = UpgradeType::BLOOD;
            card4.weight = 5;
            card4.applyEffect = [](Crow& c) { c.BLOOD *= 1.2f; };
            upgradePool.push_back(card4);

            // Lifesteal Up (Common)
            UpgradeCard card5;
            card5.title = "Lifesteal Up";
            card5.description = "Increase Lifesteal\nrate by 20%";
            card5.type = UpgradeType::BLOOD;
            card5.weight = 5;
            card5.applyEffect = [](Crow& c) { c.lifeSteal *= 1.2f; };
            upgradePool.push_back(card5);

            // Drain Rate Down (Common)
            UpgradeCard card6;
            card6.title = "Drain Less";
            card6.description = "Decrease Blood Drain \nby 10%";
            card6.type = UpgradeType::BLOOD;
            card6.weight = 5;
            card6.applyEffect = [](Crow& c) { c.bloodDrainRate *= 0.9f; };
            upgradePool.push_back(card6);

            // Attack Speed (Legendary)
            UpgradeCard card10;
            card10.title = "DPS Up";
            card10.description = "Increase Attack Speed\nby 33%";
            card10.type = UpgradeType::LEGENDARY;
            card10.weight = 1; // 5x rarer than others
            card10.applyEffect = [](Crow& c) { c.attackSpeed -= 1; };
            upgradePool.push_back(card10);
        }

        void GenerateOptions(int screenWidth, int screenHeight) {
            choosingCards.clear();
            currentState = MenuState::WAITING_FOR_INPUT;
            selectedIndex = -1;
            flashCount = 0;
            isWhiteFlash = false;

            // Weighted Selection Logic
            std::random_device rd;
            std::mt19937 gen(rd());
            
            // Create a temporary pool so we can remove items (avoid duplicates in one hand)
            std::vector<UpgradeCard> tempPool = upgradePool;
            
            int amountToPick = (tempPool.size() < 3) ? (int)tempPool.size() : 3;

            for (int i = 0; i < amountToPick; i++) {
                if (tempPool.empty()) break;

                // Calculate total weight
                int totalWeight = 0;
                for (const auto& card : tempPool) {
                    totalWeight += card.weight;
                }

                // Pick a random weight value
                std::uniform_int_distribution<> dist(0, totalWeight - 1);
                int randomValue = dist(gen);

                // Find the card corresponding to that weight
                int currentWeight = 0;
                int pickedIndex = -1;
                for (size_t j = 0; j < tempPool.size(); j++) {
                    currentWeight += tempPool[j].weight;
                    if (randomValue < currentWeight) {
                        pickedIndex = j;
                        break;
                    }
                }

                // Add to hand and remove from temp pool
                if (pickedIndex != -1) {
                    UpgradeCard pickedCard = tempPool[pickedIndex];
                    
                    // Set position logic
                    int totalWidth = (amountToPick * CARD_WIDTH) + ((amountToPick - 1) * GAP);
                    int startX = (screenWidth - totalWidth) / 2;
                    int startY = screenHeight / 2;
                    
                    pickedCard.rect = {
                        (float)(startX + (CARD_WIDTH + GAP) * i), 
                        (float)startY, 
                        (float)CARD_WIDTH, 
                        (float)CARD_HEIGHT
                    };
                    pickedCard.isHovered = false;
                    
                    choosingCards.push_back(pickedCard);
                    tempPool.erase(tempPool.begin() + pickedIndex);
                }
            }
        }

        bool Update(Crow& player) {
            float dt = GetFrameTime();

            if (currentState == MenuState::WAITING_FOR_INPUT) {
                Vector2 mousePos = GetMousePosition();

                for (size_t i = 0; i < choosingCards.size(); i++) {
                    auto& card = choosingCards[i];

                    // Check Collision
                    if (CheckCollisionPointRec(mousePos, card.rect)) {
                        card.isHovered = true;

                        // Click Handling
                        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                            // Start Animation Sequence
                            currentState = MenuState::ANIMATING_SELECTION;
                            selectedIndex = (int)i;
                            flashTimer = 0.0f;
                            flashCount = 0;
                            isWhiteFlash = true;
                            // Go to animation
                        }
                    } else {
                        card.isHovered = false;
                    }
                }
            }
            else if (currentState == MenuState::ANIMATING_SELECTION) {
                // Flash Animation Logic
                flashTimer += dt;
                if (flashTimer >= FLASH_SPEED) {
                    flashTimer = 0.0f;
                    isWhiteFlash = !isWhiteFlash; // Toggle white/normal
                    flashCount++;

                    // End Animation
                    if (flashCount >= TOTAL_FLASHES) {
                        // Apply effect
                        if (selectedIndex >= 0 && selectedIndex < (int)choosingCards.size()) {
                            choosingCards[selectedIndex].applyEffect(player);
                        }
                        return true; // Close menu
                    }
                }
            }

            return false;
        }

        // Draw Upgrade Menu
        void Draw() {
            // Darken Background
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

            // Draw Title (Yellow -> Red -> Blue rapidly)
            float time = GetTime();
            int colorStage = (int)(time * 2.0f) % 2; // 2.0f speed, 2 stages
            
            Color titleColor;
            if (colorStage == 0) titleColor = YELLOW;
            else if (colorStage == 1) titleColor = BLACK;
            // else titleColor = DARKBLUE;
            
            float fontSizeTitle = 70.0f;
            float spacingTitle = 1.0f;
            float yPosTitle = 250.0f;
            Vector2 offsetShadowTitle = { -4, 4 };
            
            const char* title = "LEVEL UP";
            Vector2 titleSize = MeasureTextEx(BoldPixels, title, fontSizeTitle, spacingTitle);
            Vector2 titlePos = { (screenWidth - titleSize.x) / 2, yPosTitle };
            Vector2 titleShadowPos = {titlePos.x + offsetShadowTitle.x, titlePos.y + offsetShadowTitle.y};
            
            DrawTextEx(BoldPixels, title, titleShadowPos, fontSizeTitle, spacingTitle, BLACK);
            DrawTextEx(BoldPixels, title, titlePos, fontSizeTitle, spacingTitle, titleColor);

            // Subtitle
            float fontSizeDesc = 50.0f;
            float spacingDesc = 1.0f;
            float yPosDesc = 350.0f;
            Vector2 offsetShadowDesc = { -4, 4 };
            
            const char* desc = "Choose an Upgrade";
            Vector2 descSize = MeasureTextEx(BoldPixels, desc, fontSizeDesc, spacingDesc);
            Vector2 descPos = { (screenWidth - descSize.x) / 2, yPosDesc };
            Vector2 descShadowPos = {descPos.x + offsetShadowDesc.x, descPos.y + offsetShadowDesc.y};
            
            DrawTextEx(BoldPixels, desc, descShadowPos, fontSizeDesc, spacingTitle, BLACK);
            DrawTextEx(BoldPixels, desc, descPos, fontSizeDesc, spacingTitle, WHITE);
            
            // Draw Cards
            for (int i = 0; i < (int)choosingCards.size(); i++) {
                const auto& card = choosingCards[i];

                bool isSelected = (currentState == MenuState::ANIMATING_SELECTION && i == selectedIndex);
                bool drawWhite = (isSelected && isWhiteFlash); // Inverted mode
                
                // If animating, don't show hover offset for non-selected cards
                bool drawHover = card.isHovered;
                if (currentState == MenuState::ANIMATING_SELECTION && i != selectedIndex) {
                    drawHover = false; 
                }

                float hoverOffset = drawHover ? -10.0f : 0.0f;
                Rectangle drawRect = card.rect;
                drawRect.y += hoverOffset;

                // Shadow
                DrawRectangleRec({card.rect.x - 10, card.rect.y + 10, card.rect.width, card.rect.height}, BLACK);
                
                // Card Background Color
                Color cardColor;
                if (drawWhite) {
                    cardColor = WHITE; // Flash Color
                } else {
                    if (card.type == UpgradeType::BLOOD) cardColor = MAROON;
                    else if (card.type == UpgradeType::MOVEMENT) cardColor = { 0, 103, 165, 255 };  // Orb Blue
                    else if (card.type == UpgradeType::LEGENDARY) cardColor = {233, 191, 4, 255};   // Gold
                    else cardColor = GRAY;

                    // Lighten if hovered
                    if (drawHover) cardColor = ColorBrightness(cardColor, 0.2f);
                }

                DrawRectangleRec(drawRect, cardColor);

                // Text Colors (Invert if flashing white)
                Color mainTextColor = drawWhite ? BLACK : RAYWHITE;
                Color shadowTextColor = drawWhite ? GRAY : BLACK;

                // Card Title
                DrawTextEx(BoldPixels, card.title.c_str(), {drawRect.x + 20, drawRect.y + 25}, 40, 1, shadowTextColor);
                DrawTextEx(BoldPixels, card.title.c_str(), {drawRect.x + 20, drawRect.y + 20}, 40, 1, mainTextColor);

                // Card Description
                DrawTextEx(BoldPixels, card.description.c_str(), {drawRect.x + 20, drawRect.y + 72}, 25, 1, shadowTextColor);
                DrawTextEx(BoldPixels, card.description.c_str(), {drawRect.x + 20, drawRect.y + 70}, 25, 1, mainTextColor);
            }
        }
};