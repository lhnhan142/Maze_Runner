#pragma once
#include <SFML/Graphics.hpp>

// =================== STRUCT NGƯỜI CHƠI ===================
struct Player {
    float x, y;
    sf::Sprite sprite;
    int animFrame;
    int spriteRow;
    sf::Clock animClock;

    int hp;
    bool isInvincible;
    sf::Clock invincibleClock;
    bool isVisible;

    float stamina;
    float maxStamina;
    float staminaRegenBonus;

    bool hasKey;

    const int spriteSize = 64;
    const int numAnimFrames = 4;
    const float animSpeed = 0.1f;
    const int INITIAL_MAX_HP = 3;
    const int MAX_MAX_HP = 5;
    const float invincibilityDuration = 2.f;
    const float MAX_STAMINA_BONUS = 0.50f;

    Player(const sf::Texture& tex, float startX, float startY);

    void updatePosition(float newX, float newY);
    void updateAnimation(bool isMoving, int newSpriteRow);
    void takeDamage();
    void updateInvincibility(float delta);
    void updateStamina(bool isSprinting, bool isMoving, float delta, float drainRate, float regenRate);
    void applyHealthPotion();
    void applyStaminaPotion();
    void reset(float startX, float startY);
};