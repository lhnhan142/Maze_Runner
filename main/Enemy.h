#pragma once
#include <SFML/Graphics.hpp>

// =================== STRUCT KẺ THÙ ===================
struct Enemy {
    float x, y;
    int direction;
    sf::Sprite sprite;
    bool isChasing;
    sf::Vector2f lastKnownPosition;
    int animFrame;
    int spriteRow;
    sf::Clock animClock;
    bool wasChasing;

    const int spriteWidth = 42;
    const int spriteHeight = 29;

    Enemy(const sf::Texture& tex, float startX, float startY)
        : x(startX), y(startY), direction(0), isChasing(false),
        animFrame(0), spriteRow(0),
        lastKnownPosition(startX, startY), wasChasing(false)
    {
        sprite.setTexture(tex);
        sprite.setTextureRect(sf::IntRect(0, 0, spriteWidth, spriteHeight));
        sprite.setOrigin(spriteWidth / 2.f, spriteHeight / 2.f);
    }
};