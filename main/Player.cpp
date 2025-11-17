#include "Player.h"

Player::Player(const sf::Texture& tex, float startX, float startY)
    : x(startX), y(startY), animFrame(0), spriteRow(0),
    hp(INITIAL_MAX_HP), isInvincible(false), isVisible(true),
    maxStamina(100.f), stamina(maxStamina), staminaRegenBonus(0.f), hasKey(false)
{
    sprite.setTexture(tex);
    sprite.setTextureRect(sf::IntRect(0, 0, spriteSize, spriteSize));
    sprite.setOrigin(spriteSize / 2.f, spriteSize / 2.f);
}

void Player::updatePosition(float newX, float newY) {
    x = newX;
    y = newY;
}

void Player::updateAnimation(bool isMoving, int newSpriteRow) {
    spriteRow = newSpriteRow;
    if (isMoving) {
        if (animClock.getElapsedTime().asSeconds() > animSpeed) {
            animFrame = (animFrame + 1) % numAnimFrames;
            animClock.restart();
        }
    }
    else {
        animFrame = 0;
    }
    sprite.setTextureRect(sf::IntRect(animFrame * spriteSize, spriteRow * spriteSize, spriteSize, spriteSize));
}

void Player::takeDamage() {
    if (isInvincible) return;

    hp--;
    isInvincible = true;
    isVisible = true;
    invincibleClock.restart();
}

void Player::updateInvincibility(float delta) {
    if (isInvincible) {
        float elapsed = invincibleClock.getElapsedTime().asSeconds();
        if (elapsed >= invincibilityDuration) {
            isInvincible = false;
            isVisible = true;
        }
        else {
            isVisible = (int(elapsed * 10) % 2) == 0;
        }
    }
}

void Player::updateStamina(bool isSprinting, bool isMoving, float delta, float drainRate, float regenRate) {
    if (isSprinting && isMoving) {
        stamina -= drainRate * delta;
        if (stamina < 0) stamina = 0;
    }
    else {
        stamina += regenRate * (1.0f + staminaRegenBonus) * delta;
        if (stamina > maxStamina) stamina = maxStamina;
    }
}

void Player::applyHealthPotion() {
    if (hp < INITIAL_MAX_HP) {
        hp++;
    }
    else if (hp < MAX_MAX_HP) {
        hp++;
    }
}

void Player::applyStaminaPotion() {
    staminaRegenBonus += 0.15f;
    if (staminaRegenBonus > MAX_STAMINA_BONUS) {
        staminaRegenBonus = MAX_STAMINA_BONUS;
    }
}

void Player::reset(float startX, float startY) {
    x = startX;
    y = startY;
    hp = INITIAL_MAX_HP;
    stamina = maxStamina;
    staminaRegenBonus = 0.f;
    hasKey = false;
    isInvincible = false;
    isVisible = true;
    animFrame = 0;
    spriteRow = 0;
    updatePosition(x, y);
    updateAnimation(false, 0);
}