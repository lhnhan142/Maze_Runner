#include "AssetManager.h"
#include <iostream>

bool AssetManager::loadAssets() {
    if (!ground.loadFromFile("../texture/ground2.png")) { std::cout << "Failed to load ground2.png\n"; return false; }
    if (!wall2.loadFromFile("../texture/wall.png")) { std::cout << "Failed to load wall.png\n"; return false; }
    if (!wall.loadFromFile("../texture/wall2.png")) { std::cout << "Failed to load wall2.png\n"; return false; }
    if (!player.loadFromFile("../texture/skeleton.png")) { std::cout << "Failed to load player.png\n"; return false; }
    if (!enemyGoblin.loadFromFile("../texture/goblin2.png")) { std::cout << "Failed to load goblin2.png\n"; return false; }
    if (!redTrap.loadFromFile("../texture/SpikeTrap.png")) { std::cout << "Failed to load SpikeTrap.png\n"; return false; }
    if (!yellowTrap.loadFromFile("../texture/FireTrap.png")) { std::cout << "Failed to load FireTrap.png\n"; return false; }
    if (!heartFull.loadFromFile("../texture/heart_full.png")) { std::cout << "Failed to load heart_full.png\n"; return false; }
    if (!heartEmpty.loadFromFile("../texture/heart_empty.png")) { std::cout << "Failed to load heart_empty.png\n"; return false; }
    if (!hudKeyIcons.loadFromFile("../texture/hud_key_icons.png")) { std::cout << "Failed to load hud_key_icons.png\n"; return false; }
    if (!staminaIcon.loadFromFile("../texture/stamina.png")) { std::cout << "Failed to load stamina.png\n"; return false; }
    if (!key.loadFromFile("../texture/key.png")) { std::cout << "Failed to load key.png\n"; return false; }
    if (!exitDoor.loadFromFile("../texture/door.png")) { std::cout << "Failed to load door.png\n"; return false; }
    if (!healthPotion.loadFromFile("../texture/healPotion.png")) { std::cout << "Failed to load healPotion.png\n"; return false; }
    if (!staminaPotion.loadFromFile("../texture/staminaPotion.png")) { std::cout << "Failed to load staminaPotion.png\n"; return false; }
    if (!background.loadFromFile("../texture/background.png")) { std::cout << "Failed to load background.png\n"; return false; }
    return true;
}
