#pragma once
#include <SFML/Graphics.hpp>

// =================== STRUCT QUẢN LÝ TÀI NGUYÊN ===================
struct AssetManager {
    sf::Texture ground;
    sf::Texture wall;
    sf::Texture wall2;
    sf::Texture player;
    sf::Texture enemyGoblin;
    sf::Texture redTrap;
    sf::Texture yellowTrap;
    sf::Texture heartFull;
    sf::Texture heartEmpty;
    sf::Texture hudKeyIcons;
    sf::Texture staminaIcon;
    sf::Texture key;
    sf::Texture exitDoor;
    sf::Texture healthPotion;
    sf::Texture staminaPotion;
    sf::Texture background;

    bool loadAssets();
};