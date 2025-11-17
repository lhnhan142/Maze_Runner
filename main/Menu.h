#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "SoundManager.h"

// =================== HÀM MENU (Hỗ trợ Audio) ===================
int showMenu(sf::RenderWindow& window, sf::Font& font,
    const std::string& title, sf::Color titleColor,
    const std::vector<std::string>& options,
    const sf::Texture& bgTexture,
    SoundManager& soundManager);