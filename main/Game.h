#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include <map>
#include <string>
#include <functional>

#include "GameTypes.h"
#include "Maze.h"
#include "Player.h"
#include "Enemy.h"
#include "Potion.h"
#include "AssetManager.h"
#include "SoundManager.h"

// =================== CLASS GAME CHÍNH ===================
class Game {
public:
    const int W = 51;
    const int H = 31;
    const int cellSize = 20;
    const float speed = 85.f;
    const double redTrapProbability = 0.08;
    const double yellowTrapProbability = 0.05;
    const float PLAYER_VISUAL_SCALE = 0.85f;
    const float ENEMY_VISUAL_SCALE = 1.0f;
    const float ENEMY_OFFSET_X = 0.f;
    const float ENEMY_OFFSET_Y = 2.f;
    const float POTION_SPAWN_PROBABILITY = 0.015;
    const float POTION_VISUAL_SCALE = 0.75f;

    Maze maze;
    Player player;
    std::vector<Enemy> enemies;
    std::vector<Potion> potions;
    bool gameOver;
    bool win;
    int level;

    sf::RenderWindow& window;
    sf::Font& font;
    const AssetManager& assets;
    SoundManager& soundManager;

    sf::View gameView;
    sf::View hudView;

    sf::Clock deltaClock, redClock, yellowClock, lockedMessageClock;
    sf::Clock redTrapAnimClock, yellowTrapAnimClock;
    bool redActive, yellowActive, showLockedMessage;
    int redTrapAnimFrame, yellowTrapAnimFrame;

    // ... (các hằng số sprite trap)
    const int redTrapSpriteSize = 32;
    const int redTrapInactiveAnimFrames = 3;
    const int redTrapActiveAnimOffset = 4;
    const int redTrapActiveAnimFrames = 9;
    const float redTrapAnimSpeed = 0.12f;

    const int yellowTrapSpriteSizeW = 32;
    const int yellowTrapSpriteSizeH = 32;
    const int yellowTrapInactiveAnimFrames = 1;
    const int yellowTrapActiveAnimOffset = 7;
    const int yellowTrapActiveAnimFrames = 3;
    const float yellowTrapAnimSpeed = 0.2f;

    const int NUM_ENEMIES = 5;
    float enemySpeed = 39.f;
    const float ENEMY_AGGRO_RANGE = cellSize * 6;
    const float ENEMY_DEAGGRO_RANGE = cellSize * 9;
    float enemyChaseSpeed = 69.f;
    const float AGGRO_RANGE_SQ = ENEMY_AGGRO_RANGE * ENEMY_AGGRO_RANGE;
    const float DEAGGRO_RANGE_SQ = ENEMY_DEAGGRO_RANGE * ENEMY_DEAGGRO_RANGE;

    float sprintSpeed = speed * 1.75f;
    float staminaDrainRate = 30.f;
    float staminaRegenRate = 15.f;
    const float STAMINA_BAR_WIDTH = 70.f;
    const float STAMINA_BAR_HEIGHT = 15.f;

    const int KEY_ICON_SIZE_W = 353;
    const int KEY_ICON_SIZE_H = 353;

    sf::Sprite groundSprite, wallSprite, wall2Sprite;
    sf::Sprite redTrapSprite, yellowTrapSprite, keySprite;
    sf::Sprite exitDoorSprite;
    sf::Sprite healthPotionSprite, staminaPotionSprite;

    sf::Text message, lockedDoorMessage, hudColon;
    sf::Text hudLevelText;
    std::vector<sf::Sprite> heartSprites;
    sf::Sprite hudKeyIconSprite, hudKeyXSprite, hudKeyVSprite;
    sf::Sprite staminaIconSprite;
    sf::RectangleShape staminaBarBg, staminaBarFg;

    sf::RectangleShape damageOverlay;
    sf::Clock damageOverlayClock;
    const float DAMAGE_OVERLAY_DURATION = 0.2f;


    Game(sf::RenderWindow& win, sf::Font& f, const AssetManager& a, SoundManager& sm, int currentLevel);
    ~Game();

    GameState run();

private:
    void initGame();
    void spawnPotions();
    void setupSprites();
    void setupHUD();

    void processInput(float delta);
    void update(float delta);
    void render();

    void updateClocks(float delta);
    void updateCamera();
    void updateEnemies(float delta, int playerCellX, int playerCellY);
    void checkCollisions(int pcx, int pcy);
    void updateHUD();

    void drawMaze();
    void drawHUD();
};