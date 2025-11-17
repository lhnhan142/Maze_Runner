#pragma once
#include <SFML/Graphics.hpp>
#include "GameTypes.h"

// =================== STRUCT BÌNH THUỐC ===================
struct Potion {
    float x, y;
    PotionType type;
    sf::Sprite sprite;
    bool isActive;

    Potion(const sf::Texture& tex, float startX, float startY, PotionType t)
        : x(startX), y(startY), type(t), isActive(true)
    {
        sprite.setTexture(tex);
        sprite.setTextureRect(sf::IntRect(0, 0, 32, 32));
    }
};