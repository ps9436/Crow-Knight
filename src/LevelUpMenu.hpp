#pragma once
#include <raylib.h>
#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <random>
#include "Crow.hpp"

enum class UpgradeType {
    OFFENSE,    // Red
    UTILITY     // Blue
};

struct UpgradeCard {
    std::string title;
    std::string description;
    UpgradeType type;
    Rectangle rect;
    bool isHovered;

    std::function<void(Crow&)> applyEffect;
};

class LevelUpMenu {
    private:
        std::vector<UpgradeCard> choosingCards; // 3 random cards 
        std::vector<UpgradeCard> upgradePool;   // All possible cards (upgrades)

        const int CARD_WIDTH = 200;
        const int CARD_HEIGHT = 300;
        const int GAP = 50;

    public:
        void Init() {
            choosingCards.clear();

            // All Upgrades
            // Speed Up
            UpgradeCard card1;
            card1.title = "Speed Up";
            card1.description = "Increase Speed\nby 20%.";
            card1.type = UpgradeType::UTILITY;
            card1.applyEffect = [](Crow& c) { c.speed *= 1.2f; }; // Direct lambda access
            upgradePool.push_back(card1);

            // Dash Up
            UpgradeCard card2;
            card2.title = "Fast Dash";
            card2.description = "Increase Dash Speed\nby 20%.";
            card2.type = UpgradeType::UTILITY;
            card2.applyEffect = [](Crow& c) { c.dashDuration *= 0.8f; };
            upgradePool.push_back(card2);

            // Blood Up
            UpgradeCard card3;
            card3.title = "Blood Up";
            card3.description = "Increase Health\nby 20%.";
            card3.type = UpgradeType::OFFENSE;
            card3.applyEffect = [](Crow& c) { c.BLOOD *= 1.2f; };
            upgradePool.push_back(card3);

            // Attack Speed
            UpgradeCard card4;
            card4.title = "Attack Speed Up";
            card4.description = "Increase Attack Speed\nby 33%.";
            card4.type = UpgradeType::OFFENSE;
            card4.applyEffect = [](Crow& c) { c.attackSpeed -= 1; };
            upgradePool.push_back(card4);
        }

        void GenerateOptions(int screenWidth, int screenHeight) {
            choosingCards.clear();

            // Shuffle upgrades
            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(upgradePool.begin(), upgradePool.end(), g);

            int amountToPick = (upgradePool.size() < 3) ? (int)upgradePool.size() : 3;

            int totalWidth = (amountToPick * CARD_WIDTH) + ((amountToPick - 1) * GAP);
            int startX = (screenWidth - totalWidth) / 2;
            int startY = (screenHeight - CARD_HEIGHT) / 2;

            for (int i = 0; i < amountToPick; i++) {
                // Copy the card from the pool
                UpgradeCard card = upgradePool[i];

                // Set position on screen
                card.rect = {
                    (float)(startX + (CARD_WIDTH + GAP) * i), 
                    (float)startY, 
                    (float)CARD_WIDTH, 
                    (float)CARD_HEIGHT
                };
                card.isHovered = false;

                choosingCards.push_back(card);
            }
        }

        bool Update(Crow& player) {
            Vector2 mousePos = GetMousePosition();

            for (auto& card : choosingCards) {
                // Check Collision
                if (CheckCollisionPointRec(mousePos, card.rect)) {
                    card.isHovered = true;

                    // Click Handling
                    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                        card.applyEffect(player);   // Apply the instructions
                        return true;
                    }
                } else {
                    card.isHovered = false;
                }
            }
            return false;
        }

        void Draw() {
            // Darken Background
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.7f));

            // Draw Title
            DrawText("LEVEL UP!", GetScreenWidth()/2 - 100, 50, 40, YELLOW);
            DrawText("Choose an upgrade", GetScreenWidth()/2 - 100, 100, 20, WHITE);

            // Draw Cards
            for (const auto& card : choosingCards) {
                // Move up 10 pixels if hoevered
                float hoverOffset = card.isHovered ? -10.0f : 0.0f;

                Rectangle drawRect = card.rect;
                drawRect.y += hoverOffset;

                // Shadow
                DrawRectangleRec({card.rect.x + 10, card.rect.y + 10, card.rect.width, card.rect.height}, BLACK);
                // Card
                Color cardColor = (card.type == UpgradeType::OFFENSE) ? MAROON : DARKBLUE;
                // If hovered, make it slightly lighter to indicate interaction
                if (card.isHovered) cardColor = ColorBrightness(cardColor, 0.2f);

                DrawRectangleRec(drawRect, cardColor);
            
                // Outline
                DrawRectangleLinesEx(drawRect, 3, RAYWHITE);

                // Title
                DrawText(card.title.c_str(), 
                        (int)(drawRect.x + 20), 
                        (int)(drawRect.y + 20), 
                        20, WHITE);

                // Description
                DrawText(card.description.c_str(), 
                        (int)(drawRect.x + 20), 
                        (int)(drawRect.y + 60), 
                        16, LIGHTGRAY);
            }
        }
};