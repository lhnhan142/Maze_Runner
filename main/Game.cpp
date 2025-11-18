#include "Game.h"
#include "GameUtils.h" // Cần cho hasLineOfSight
#include "Menu.h"      // Cần cho showMenu khi game over
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <ctime>

// =================== ĐỊNH NGHĨA HÀM THÀNH VIÊN ===================

Game::Game(sf::RenderWindow& win, sf::Font& f, const AssetManager& a, SoundManager& sm, int currentLevel)
    : maze(W, H),
    player(a.player, 0, 0),
    window(win),
    font(f),
    assets(a),
    soundManager(sm),
    level(currentLevel),
    gameView(sf::FloatRect(0, 0, win.getSize().x, win.getSize().y)),
    hudView(sf::FloatRect(0, 0, win.getSize().x, win.getSize().y))
{
    gameView.setSize(win.getSize().x * 0.2f, win.getSize().y * 0.2f);
    setupSprites();
    setupHUD();
    initGame();
    soundManager.startGameMusic();
}

Game::~Game() {
    soundManager.stopGameMusic();
    soundManager.stopWalking();
}

void Game::updateClocks(float delta) {
    if (redClock.getElapsedTime().asSeconds() >= 1.36f) {
        redActive = !redActive;
        redClock.restart();
        if (redActive) redTrapAnimFrame = redTrapActiveAnimOffset;
        else redTrapAnimFrame = 0;
    }
    if (yellowClock.getElapsedTime().asSeconds() >= 3.36f) {
        yellowActive = !yellowActive;
        yellowClock.restart();
        if (yellowActive) yellowTrapAnimFrame = yellowTrapActiveAnimOffset;
        else yellowTrapAnimFrame = 0;
    }
    if (showLockedMessage && lockedMessageClock.getElapsedTime().asSeconds() > 3.0f) {
        showLockedMessage = false;
    }
    if (redTrapAnimClock.getElapsedTime().asSeconds() > redTrapAnimSpeed) {
        if (redActive) {
            int currentRelativeFrame = redTrapAnimFrame - redTrapActiveAnimOffset;
            currentRelativeFrame = (currentRelativeFrame + 1) % redTrapActiveAnimFrames;
            redTrapAnimFrame = currentRelativeFrame + redTrapActiveAnimOffset;
        }
        else {
            redTrapAnimFrame = (redTrapAnimFrame + 1) % redTrapInactiveAnimFrames;
        }
        redTrapAnimClock.restart();
    }
    if (yellowTrapAnimClock.getElapsedTime().asSeconds() > yellowTrapAnimSpeed) {
        if (yellowActive) {
            int currentRelativeFrame = yellowTrapAnimFrame - yellowTrapActiveAnimOffset;
            currentRelativeFrame = (currentRelativeFrame + 1) % yellowTrapActiveAnimFrames;
            yellowTrapAnimFrame = currentRelativeFrame + yellowTrapActiveAnimOffset;
        }
        else {
            yellowTrapAnimFrame = (yellowTrapAnimFrame + 1) % yellowTrapInactiveAnimFrames;
        }
        yellowTrapAnimClock.restart();
    }
}

void Game::updateCamera() {
    float viewX = player.x + cellSize / 2.f;
    float viewY = player.y + cellSize / 2.f;

    float viewSizeX = gameView.getSize().x;
    float viewSizeY = gameView.getSize().y;
    float mapWidth = W * cellSize;
    float mapHeight = H * cellSize;

    float minViewX = (mapWidth < viewSizeX) ? (mapWidth / 2.f) : (viewSizeX / 2.f);
    float maxViewX = (mapWidth < viewSizeX) ? (mapWidth / 2.f) : (mapWidth - viewSizeX / 2.f);
    float minViewY = (mapHeight < viewSizeY) ? (mapHeight / 2.f) : (viewSizeY / 2.f);
    float maxViewY = (mapHeight < viewSizeY) ? (mapHeight / 2.f) : (mapHeight - viewSizeY / 2.f);

    viewX = std::max(minViewX, std::min(viewX, maxViewX));
    viewY = std::max(minViewY, std::min(viewY, maxViewY));
    gameView.setCenter(viewX, viewY);
}

void Game::updateEnemies(float delta, int playerCellX, int playerCellY) {
    const float enemyCollisionPadding = 3.f;
    const int GOBLIN_SPRITE_WIDTH = 42;
    const int GOBLIN_SPRITE_HEIGHT = 29;

    float playerCenterX = player.x + cellSize / 2.f;
    float playerCenterY = player.y + cellSize / 2.f;

    for (auto& enemy : enemies) {
        enemy.wasChasing = enemy.isChasing;

        float oldX = enemy.x, oldY = enemy.y;
        float enemyCenterX = enemy.x + cellSize / 2.f;
        float enemyCenterY = enemy.y + cellSize / 2.f;
        int ecx = int(enemyCenterX / cellSize);
        int ecy = int(enemyCenterY / cellSize);

        float distToPlayerSq = (playerCenterX - enemyCenterX) * (playerCenterX - enemyCenterX) +
            (playerCenterY - enemyCenterY) * (playerCenterY - enemyCenterY);

        int ecx_clamped = std::max(0, std::min(ecx, W - 1));
        int ecy_clamped = std::max(0, std::min(ecy, H - 1));

        bool hasLOS = hasLineOfSight(maze, ecx_clamped, ecy_clamped, playerCellX, playerCellY);
        if (enemy.isChasing) {
            if (hasLOS) {
                enemy.lastKnownPosition = sf::Vector2f(playerCenterX, playerCenterY);
                if (distToPlayerSq > DEAGGRO_RANGE_SQ) {
                    enemy.isChasing = false;
                }
            }
            else {
                float distToLastPosSq = (enemy.lastKnownPosition.x - enemyCenterX) * (enemy.lastKnownPosition.x - enemyCenterX) +
                    (enemy.lastKnownPosition.y - enemyCenterY) * (enemy.lastKnownPosition.y - enemyCenterY);
                if (distToLastPosSq < (cellSize * cellSize / 4.f)) {
                    enemy.isChasing = false;
                }
            }
        }
        else {
            if (distToPlayerSq < AGGRO_RANGE_SQ && hasLOS) {
                enemy.isChasing = true;
                enemy.lastKnownPosition = sf::Vector2f(playerCenterX, playerCenterY);
            }
        }

        if (enemy.isChasing && !enemy.wasChasing) {
            if (rand() % 2 == 0) soundManager.playGoblinLaugh();
            else soundManager.playTindeck();
        }

        float currentSpeed = enemy.isChasing ? enemyChaseSpeed : enemySpeed;
        float nx_enemy = enemy.x, ny_enemy = enemy.y;
        bool didMove = false;
        float threshold = 2.0f;

        if (enemy.isChasing) {
            float targetX = enemy.lastKnownPosition.x;
            float targetY = enemy.lastKnownPosition.y;
            float dx = targetX - enemyCenterX;
            float dy = targetY - enemyCenterY;
            bool preferMoveX = (std::abs(dx) > std::abs(dy));

            for (int i = 0; i < 2; ++i) {
                if (preferMoveX) {
                    if (dx > threshold) nx_enemy = enemy.x + currentSpeed * delta;
                    else if (dx < -threshold) nx_enemy = enemy.x - currentSpeed * delta;
                    else { preferMoveX = !preferMoveX; continue; }

                    bool collisionX = false;
                    int topCellY = int((enemy.y + enemyCollisionPadding) / cellSize);
                    int bottomCellY = int((enemy.y + cellSize - enemyCollisionPadding) / cellSize);

                    if (nx_enemy > enemy.x) {
                        int rightCellX = int((nx_enemy + cellSize - enemyCollisionPadding) / cellSize);
                        if (rightCellX >= W || (topCellY >= 0 && topCellY < H && maze.walls[topCellY][rightCellX]) || (bottomCellY >= 0 && bottomCellY < H && maze.walls[bottomCellY][rightCellX])) collisionX = true;
                    }
                    else {
                        int leftCellX = int((nx_enemy + enemyCollisionPadding) / cellSize);
                        if (leftCellX < 0 || (topCellY >= 0 && topCellY < H && maze.walls[topCellY][leftCellX]) || (bottomCellY >= 0 && bottomCellY < H && maze.walls[bottomCellY][leftCellX])) collisionX = true;
                    }

                    if (!collisionX) {
                        enemy.x = nx_enemy; didMove = true;
                    }

                }
                else {
                    if (dy > threshold) ny_enemy = enemy.y + currentSpeed * delta;
                    else if (dy < -threshold) ny_enemy = enemy.y - currentSpeed * delta;
                    else { preferMoveX = !preferMoveX; continue; }

                    bool collisionY = false;
                    int leftCellX = int((enemy.x + enemyCollisionPadding) / cellSize);
                    int rightCellX = int((enemy.x + cellSize - enemyCollisionPadding) / cellSize);

                    if (ny_enemy > enemy.y) {
                        int bottomCellY = int((ny_enemy + cellSize - enemyCollisionPadding) / cellSize);
                        if (bottomCellY >= H || (leftCellX >= 0 && leftCellX < W && maze.walls[bottomCellY][leftCellX]) || (rightCellX >= 0 && rightCellX < W && maze.walls[bottomCellY][rightCellX])) collisionY = true;
                    }
                    else {
                        int topCellY = int((ny_enemy + enemyCollisionPadding) / cellSize);
                        if (topCellY < 0 || (leftCellX >= 0 && leftCellX < W && maze.walls[topCellY][leftCellX]) || (rightCellX >= 0 && rightCellX < W && maze.walls[topCellY][rightCellX])) collisionY = true;
                    }

                    if (!collisionY) {
                        enemy.y = ny_enemy; didMove = true;
                    }
                }
                if (didMove) break;
                preferMoveX = !preferMoveX;
            }
            if (!didMove) enemy.isChasing = false;
        }

        if (!enemy.isChasing) {
            nx_enemy = enemy.x; ny_enemy = enemy.y;
            if (enemy.direction == 0) ny_enemy -= currentSpeed * delta;
            else if (enemy.direction == 1) ny_enemy += currentSpeed * delta;
            else if (enemy.direction == 2) nx_enemy -= currentSpeed * delta;
            else if (enemy.direction == 3) nx_enemy += currentSpeed * delta;

            bool wallHit = false;

            if (enemy.direction == 2 || enemy.direction == 3) {
                int topCellY = int((enemy.y + enemyCollisionPadding) / cellSize);
                int bottomCellY = int((enemy.y + cellSize - enemyCollisionPadding) / cellSize);

                if (enemy.direction == 3) {
                    int rightCellX = int((nx_enemy + cellSize - enemyCollisionPadding) / cellSize);
                    if (rightCellX >= W || (topCellY >= 0 && topCellY < H && maze.walls[topCellY][rightCellX]) || (bottomCellY >= 0 && bottomCellY < H && maze.walls[bottomCellY][rightCellX])) wallHit = true;
                }
                else {
                    int leftCellX = int((nx_enemy + enemyCollisionPadding) / cellSize);
                    if (leftCellX < 0 || (topCellY >= 0 && topCellY < H && maze.walls[topCellY][leftCellX]) || (bottomCellY >= 0 && bottomCellY < H && maze.walls[bottomCellY][leftCellX])) wallHit = true;
                }
                if (!wallHit) enemy.x = nx_enemy;

            }
            else if (enemy.direction == 0 || enemy.direction == 1) {
                int leftCellX = int((enemy.x + enemyCollisionPadding) / cellSize);
                int rightCellX = int((enemy.x + cellSize - enemyCollisionPadding) / cellSize);

                if (enemy.direction == 1) {
                    int bottomCellY = int((ny_enemy + cellSize - enemyCollisionPadding) / cellSize);
                    if (bottomCellY >= H || (leftCellX >= 0 && leftCellX < W && maze.walls[bottomCellY][leftCellX]) || (rightCellX >= 0 && rightCellX < W && maze.walls[bottomCellY][rightCellX])) wallHit = true;
                }
                else {
                    int topCellY = int((ny_enemy + enemyCollisionPadding) / cellSize);
                    if (topCellY < 0 || (leftCellX >= 0 && leftCellX < W && maze.walls[topCellY][leftCellX]) || (rightCellX >= 0 && rightCellX < W && maze.walls[topCellY][rightCellX])) wallHit = true;
                }
                if (!wallHit) enemy.y = ny_enemy;
            }
            if (wallHit) enemy.direction = rand() % 4;
        }

        float moveX = enemy.x - oldX;
        float moveY = enemy.y - oldY;
        bool enemyIsMoving = (std::abs(moveX) > 0.1f) || (std::abs(moveY) > 0.1f);
        if (enemyIsMoving) {
            if (std::abs(moveX) > std::abs(moveY)) enemy.spriteRow = (moveX > 0) ? 3 : 2;
            else enemy.spriteRow = (moveY > 0) ? 0 : 1;
            if (enemy.animClock.getElapsedTime().asSeconds() > player.animSpeed) {
                enemy.animFrame = (enemy.animFrame + 1) % player.numAnimFrames;
                enemy.animClock.restart();
            }
        }
        else enemy.animFrame = 0;

        enemy.sprite.setTextureRect(sf::IntRect(enemy.animFrame * GOBLIN_SPRITE_WIDTH, enemy.spriteRow * GOBLIN_SPRITE_HEIGHT, GOBLIN_SPRITE_WIDTH, GOBLIN_SPRITE_HEIGHT));
        enemy.sprite.setPosition(
            enemy.x + cellSize / 2.f + ENEMY_OFFSET_X,
            enemy.y + cellSize / 2.f + ENEMY_OFFSET_Y);
    }
}


void Game::initGame() {
    enemySpeed = 39.f + (level * 2.f);
    enemyChaseSpeed = 69.f + (level * 3.f);
    int numEnemies = std::min(NUM_ENEMIES + (level - 1), 15);

    maze.generate();
    maze.generateTraps(redTrapProbability, yellowTrapProbability);

    std::mt19937 keyGen(time(0) + 1);
    std::vector<std::pair<int, int>> validKeySpots;
    for (int y = 1; y < H - 1; y++) {
        for (int x = 1; x < W - 1; x++) {
            if (!maze.walls[y][x] && maze.traps[y][x] == 0 &&
                (x != maze.entrance.first || y != maze.entrance.second) &&
                (x != maze.exit.first || y != maze.exit.second)) {
                validKeySpots.push_back({ x, y });
            }
        }
    }
    if (validKeySpots.empty()) maze.key = { 1, 1 };
    else {
        std::uniform_int_distribution<> keyDis(0, validKeySpots.size() - 1);
        maze.key = validKeySpots[keyDis(keyGen)];
    }

    player.reset(maze.entrance.first * cellSize, maze.entrance.second * cellSize);
    player.sprite.setPosition(
        player.x + cellSize / 2.f,
        player.y + cellSize / 2.f
    );

    enemies.clear();
    const int ENEMY_SPAWN_SAFE_RADIUS = 5;
    std::vector<std::pair<int, int>> filteredEnemySpots;
    int startX = maze.entrance.first;
    int startY = maze.entrance.second;
    for (const auto& spot : validKeySpots) {
        if (std::abs(spot.first - startX) + std::abs(spot.second - startY) > ENEMY_SPAWN_SAFE_RADIUS) {
            filteredEnemySpots.push_back(spot);
        }
    }

    std::shuffle(filteredEnemySpots.begin(), filteredEnemySpots.end(), keyGen);
    for (int i = 0; i < numEnemies && i < filteredEnemySpots.size(); ++i) {
        int ex = filteredEnemySpots[i].first;
        int ey = filteredEnemySpots[i].second;

        enemies.emplace_back(assets.enemyGoblin, ex * cellSize, ey * cellSize);
        enemies.back().direction = keyGen() % 4;

        float enemyScale = (cellSize / 42.f) * ENEMY_VISUAL_SCALE;
        enemies.back().sprite.setScale(enemyScale, enemyScale);

        enemies.back().sprite.setPosition(
            ex * cellSize + cellSize / 2.f + ENEMY_OFFSET_X,
            ey * cellSize + cellSize / 2.f + ENEMY_OFFSET_Y
        );

        enemies.back().x = ex * cellSize;
        enemies.back().y = ey * cellSize;
    }

    spawnPotions();

    gameOver = false;
    win = false;
    redActive = true;
    yellowActive = true;
    showLockedMessage = false;
    redTrapAnimFrame = 0;
    yellowTrapAnimFrame = 0;

    deltaClock.restart();
    redClock.restart();
    yellowClock.restart();
    redTrapAnimClock.restart();
    yellowTrapAnimClock.restart();

    message.setString("");
    lockedDoorMessage.setString("Door is locked! Find the key.");
}

void Game::spawnPotions() {
    potions.clear();
    std::mt19937 gen(time(0) + 2);
    std::uniform_real_distribution<> probDis(0, 1);
    std::uniform_int_distribution<> typeDis(0, 1);

    for (int y = 1; y < H - 1; y++) {
        for (int x = 1; x < W - 1; x++) {
            if (!maze.walls[y][x] && maze.traps[y][x] == 0 &&
                (x != maze.entrance.first || y != maze.entrance.second) &&
                (x != maze.exit.first || y != maze.exit.second) &&
                (x != maze.key.first || y != maze.key.second) &&
                probDis(gen) < POTION_SPAWN_PROBABILITY)
            {
                PotionType type = typeDis(gen) == 0 ? PotionType::Health : PotionType::Stamina;
                const sf::Texture& tex = (type == PotionType::Health) ? assets.healthPotion : assets.staminaPotion;

                potions.emplace_back(tex, x * cellSize, y * cellSize, type);

                float baseScale = cellSize / 32.f;
                float finalScale = baseScale * POTION_VISUAL_SCALE;

                potions.back().sprite.setScale(finalScale, finalScale);
            }
        }
    }
}


void Game::setupSprites() {
    groundSprite.setTexture(assets.ground);
    groundSprite.setScale(cellSize / float(assets.ground.getSize().x), cellSize / float(assets.ground.getSize().y));

    wallSprite.setTexture(assets.wall);
    wallSprite.setScale(cellSize / float(assets.wall.getSize().x), cellSize / float(assets.wall.getSize().y));

    wall2Sprite.setTexture(assets.wall2);
    wall2Sprite.setScale(cellSize / float(assets.wall2.getSize().x), cellSize / float(assets.wall2.getSize().y));

    redTrapSprite.setTexture(assets.redTrap);
    redTrapSprite.setScale(cellSize / float(redTrapSpriteSize), cellSize / float(redTrapSpriteSize));

    yellowTrapSprite.setTexture(assets.yellowTrap);
    yellowTrapSprite.setScale(cellSize / float(yellowTrapSpriteSizeW), cellSize / float(yellowTrapSpriteSizeW));

    keySprite.setTexture(assets.key);
    keySprite.setScale(cellSize / float(assets.key.getSize().x), cellSize / float(assets.key.getSize().y));

    exitDoorSprite.setTexture(assets.exitDoor);
    float doorScale = cellSize / 16.0f;
    exitDoorSprite.setScale(doorScale, doorScale);

    player.sprite.setScale(
        (cellSize / float(player.spriteSize)) * PLAYER_VISUAL_SCALE,
        (cellSize / float(player.spriteSize)) * PLAYER_VISUAL_SCALE
    );

    healthPotionSprite.setTexture(assets.healthPotion);
    staminaPotionSprite.setTexture(assets.staminaPotion);
}

void Game::setupHUD() {
    message.setFont(font); message.setCharacterSize(40); message.setStyle(sf::Text::Bold);
    lockedDoorMessage.setFont(font); lockedDoorMessage.setCharacterSize(12); lockedDoorMessage.setFillColor(sf::Color::Yellow);

    float currentHUDX = 10.f;
    float currentHUDY = 10.f;

    float heartScale = 0.45f;
    float heartSpacing = 2.f;
    heartSprites.resize(player.MAX_MAX_HP);
    for (int i = 0; i < player.MAX_MAX_HP; ++i) {
        heartSprites[i].setTexture(assets.heartFull);
        heartSprites[i].setScale(heartScale, heartScale);
        heartSprites[i].setPosition(currentHUDX, currentHUDY);
        currentHUDX += (assets.heartFull.getSize().x * heartScale + heartSpacing);
    }
    float heartSpriteHeight = assets.heartFull.getSize().y * heartScale;

    float staminaRowY = currentHUDY + heartSpriteHeight + 5.f;
    float staminaRowX = 10.f;
    float staminaRowCenterY = staminaRowY + heartSpriteHeight / 2.f;
    float staminaIconScale = heartSpriteHeight / assets.staminaIcon.getSize().y;
    staminaIconSprite.setTexture(assets.staminaIcon);
    staminaIconSprite.setScale(staminaIconScale, staminaIconScale);
    staminaIconSprite.setPosition(staminaRowX, staminaRowY);
    float staminaBarX = staminaRowX + (assets.staminaIcon.getSize().x * staminaIconScale) + 3.f;
    float staminaBarY = staminaRowCenterY - (STAMINA_BAR_HEIGHT / 2.f);
    staminaBarBg.setSize(sf::Vector2f(STAMINA_BAR_WIDTH, STAMINA_BAR_HEIGHT));
    staminaBarBg.setFillColor(sf::Color(50, 50, 50));
    staminaBarBg.setOutlineThickness(1);
    staminaBarBg.setOutlineColor(sf::Color::Black);
    staminaBarBg.setPosition(staminaBarX, staminaBarY);
    staminaBarFg.setSize(sf::Vector2f(STAMINA_BAR_WIDTH, STAMINA_BAR_HEIGHT));
    staminaBarFg.setFillColor(sf::Color::Yellow);
    staminaBarFg.setPosition(staminaBarX, staminaBarY);

    float keyRowY = staminaRowY + heartSpriteHeight + 5.f;
    float keyRowX = 10.f;
    float keyRowCenterY = keyRowY + heartSpriteHeight / 2.f;
    float keyIconScale = heartSpriteHeight / assets.key.getSize().y;
    hudKeyIconSprite.setTexture(assets.key);
    hudKeyIconSprite.setScale(keyIconScale, keyIconScale);
    hudKeyIconSprite.setPosition(keyRowX, keyRowY);
    hudColon.setFont(font); hudColon.setCharacterSize(20);
    hudColon.setStyle(sf::Text::Bold); hudColon.setFillColor(sf::Color::White);
    hudColon.setString(":");
    float colonX = keyRowX + (assets.key.getSize().x * keyIconScale) + 3.f;
    sf::FloatRect colonBounds = hudColon.getLocalBounds();
    float colonCenterYOffset = colonBounds.top + colonBounds.height / 2.0f;
    float colonY = keyRowCenterY - colonCenterYOffset;
    hudColon.setPosition(colonX, colonY);
    float checkIconScale = heartSpriteHeight / KEY_ICON_SIZE_H;
    float checkIconX = colonX + colonBounds.width + 3.f;
    hudKeyXSprite.setTexture(assets.hudKeyIcons);
    hudKeyXSprite.setTextureRect(sf::IntRect(KEY_ICON_SIZE_W, 0, KEY_ICON_SIZE_W, KEY_ICON_SIZE_H));
    hudKeyXSprite.setScale(checkIconScale, checkIconScale);
    hudKeyXSprite.setPosition(checkIconX, keyRowY);
    hudKeyVSprite.setTexture(assets.hudKeyIcons);
    hudKeyVSprite.setTextureRect(sf::IntRect(0, 0, KEY_ICON_SIZE_W, KEY_ICON_SIZE_H));
    hudKeyVSprite.setScale(checkIconScale, checkIconScale);
    hudKeyVSprite.setPosition(checkIconX, keyRowY);

    hudLevelText.setFont(font);
    hudLevelText.setCharacterSize(20);
    hudLevelText.setFillColor(sf::Color::White);
    hudLevelText.setString("Level: " + std::to_string(level));

    sf::FloatRect levelTextBounds = hudLevelText.getLocalBounds();
    hudLevelText.setOrigin(levelTextBounds.left + levelTextBounds.width, 0);
    hudLevelText.setPosition(window.getSize().x - 10.f, 10.f);

    damageOverlay.setSize(sf::Vector2f(window.getSize()));
    damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));
    damageOverlay.setPosition(0, 0);
}

GameState Game::run() {
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                soundManager.stopGameMusic();
                return GameState::Exit;
            }
        }

        float delta = deltaClock.restart().asSeconds();

        if (!gameOver) {
            processInput(delta);
            update(delta);
        }

        render();

        if (gameOver) {
            soundManager.stopGameMusic();
            soundManager.stopWalking();

            sf::View originalView = window.getView();
            window.setView(hudView);

            std::vector<std::string> options;
            std::string title = message.getString();
            sf::Color color = message.getFillColor();
            int choice;

            if (win) {
                options = { "Continue", "Exit" };
                choice = showMenu(window, font, title, color, options, assets.background, soundManager);
                window.setView(originalView);

                if (choice == 0) return GameState::NextLevel;
                else return GameState::Exit;
            }
            else {
                options = { "Play Again", "Return to Menu" };
                choice = showMenu(window, font, title, color, options, assets.background, soundManager);
                window.setView(originalView);

                if (choice == 0) return GameState::PlayAgain;
                else return GameState::ReturnToMenu;
            }
        }
    }
    soundManager.stopGameMusic();
    soundManager.stopWalking();
    return GameState::Exit;
}

void Game::processInput(float delta) {
    if (gameOver) return;

    float oldX = player.x;
    float oldY = player.y;

    float currentMoveSpeed = speed;
    float nx = player.x, ny = player.y;
    bool isMoving = false;
    bool isSprinting = false;
    int spriteRow = player.spriteRow;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        ny -= speed * delta;
        spriteRow = 0; // Hàng 0: Đi lên
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        ny += speed * delta;
        spriteRow = 2; // Hàng 2: Đi xuống
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        nx -= speed * delta;
        spriteRow = 1; // Hàng 1: Đi qua trái
        isMoving = true;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        nx += speed * delta;
        spriteRow = 3; // Hàng 3: Đi qua phải
        isMoving = true;
    }

    if (isMoving && sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) {
        if (player.stamina > 0) {
            isSprinting = true;
            currentMoveSpeed = sprintSpeed;
        }
    }

    if (isSprinting) {
        nx = player.x; ny = player.y;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            ny -= currentMoveSpeed * delta;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            ny += currentMoveSpeed * delta;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            nx -= currentMoveSpeed * delta;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            nx += currentMoveSpeed * delta;
        }
    }

    player.updateAnimation(isMoving, spriteRow);
    player.updateStamina(isSprinting, isMoving, delta, staminaDrainRate, staminaRegenRate);

    if (isMoving) soundManager.startWalking();
    else soundManager.stopWalking();

    const float collisionPadding = 5.f;

    bool collisionX = false;
    int topCellY = int((player.y + collisionPadding) / cellSize);
    int bottomCellY = int((player.y + cellSize - collisionPadding) / cellSize);

    if (nx > player.x) {
        int rightCellX = int((nx + cellSize - collisionPadding) / cellSize);
        if (rightCellX >= W || (topCellY >= 0 && topCellY < H && maze.walls[topCellY][rightCellX]) || (bottomCellY >= 0 && bottomCellY < H && maze.walls[bottomCellY][rightCellX])) {
            collisionX = true;
        }
    }
    else if (nx < player.x) {
        int leftCellX = int((nx + collisionPadding) / cellSize);
        if (leftCellX < 0 || (topCellY >= 0 && topCellY < H && maze.walls[topCellY][leftCellX]) || (bottomCellY >= 0 && bottomCellY < H && maze.walls[bottomCellY][leftCellX])) {
            collisionX = true;
        }
    }

    int centerNx = int((nx + cellSize / 2.f) / cellSize);
    int centerNy = int((player.y + cellSize / 2.f) / cellSize);
    bool isExitCell_X = (centerNx == maze.exit.first && centerNy == maze.exit.second);

    if (!collisionX && (!isExitCell_X || player.hasKey)) {
        player.x = nx;
    }
    else {
        player.x = oldX;
    }

    bool collisionY = false;
    int leftCellX = int((player.x + collisionPadding) / cellSize);
    int rightCellX = int((player.x + cellSize - collisionPadding) / cellSize);

    if (ny > player.y) {
        int bottomCellY = int((ny + cellSize - collisionPadding) / cellSize);
        if (bottomCellY >= H || (leftCellX >= 0 && leftCellX < W && maze.walls[bottomCellY][leftCellX]) || (rightCellX >= 0 && rightCellX < W && maze.walls[bottomCellY][rightCellX])) {
            collisionY = true;
        }
    }
    else if (ny < player.y) {
        int topCellY = int((ny + collisionPadding) / cellSize);
        if (topCellY < 0 || (leftCellX >= 0 && leftCellX < W && maze.walls[topCellY][leftCellX]) || (rightCellX >= 0 && rightCellX < W && maze.walls[topCellY][rightCellX])) {
            collisionY = true;
        }
    }

    centerNx = int((player.x + cellSize / 2.f) / cellSize);
    centerNy = int((ny + cellSize / 2.f) / cellSize);
    bool isExitCell_Y = (centerNx == maze.exit.first && centerNy == maze.exit.second);

    if (!collisionY && (!isExitCell_Y || player.hasKey)) {
        player.y = ny;
    }
    else {
        player.y = oldY;
    }

    player.updatePosition(player.x, player.y);

    player.sprite.setPosition(
        player.x + cellSize / 2.f,
        player.y + cellSize / 2.f
    );
}

void Game::update(float delta) {
    updateClocks(delta);
    player.updateInvincibility(delta);

    float elapsedOverlay = damageOverlayClock.getElapsedTime().asSeconds();
    if (elapsedOverlay < DAMAGE_OVERLAY_DURATION) {
        float maxAlpha = 150.0f;
        float normalizedTime = elapsedOverlay / DAMAGE_OVERLAY_DURATION;
        float alpha = maxAlpha * (1.0f - normalizedTime);

        if (alpha < 0) alpha = 0;

        damageOverlay.setFillColor(sf::Color(255, 0, 0, (sf::Uint8)alpha));
    }
    else {
        damageOverlay.setFillColor(sf::Color(255, 0, 0, 0));
    }

    int pcx = int((player.x + cellSize / 2.f) / cellSize);
    int pcy = int((player.y + cellSize / 2.f) / cellSize);
    pcx = std::max(0, std::min(pcx, W - 1));
    pcy = std::max(0, std::min(pcy, H - 1));

    updateEnemies(delta, pcx, pcy);

    checkCollisions(pcx, pcy);
    updateCamera();
    updateHUD();
}

void Game::checkCollisions(int pcx, int pcy) {
    bool onTrap = (maze.traps[pcy][pcx] == 1 && redActive) || (maze.traps[pcy][pcx] == 2 && yellowActive);

    bool onEnemy = false;
    if (!player.isInvincible) {
        for (auto& enemy : enemies) {
            int ecx = int((enemy.x + cellSize / 2.f) / cellSize);
            int ecy = int((enemy.y + cellSize / 2.f) / cellSize);
            if (pcx == ecx && pcy == ecy) {
                onEnemy = true;
                break;
            }
        }
    }

    if (onTrap || onEnemy) {
        if (!player.isInvincible) {
            soundManager.playDamage();
            player.takeDamage();
            damageOverlayClock.restart();
            damageOverlay.setFillColor(sf::Color(255, 0, 0, 150));

            if (player.hp <= 0) {
                gameOver = true;
                win = false;
                message.setString("YOU DIED!");
                message.setFillColor(sf::Color::Red);
            }
        }
    }

    if (!player.hasKey && pcx == maze.key.first && pcy == maze.key.second) {
        player.hasKey = true;
        soundManager.playKeyPickup();
    }

    for (auto& potion : potions) {
        if (potion.isActive) {
            int potX = int((potion.x + cellSize / 2.f) / cellSize);
            int potY = int((potion.y + cellSize / 2.f) / cellSize);

            if (pcx == potX && pcy == potY) {
                potion.isActive = false;
                soundManager.playHeal();

                if (potion.type == PotionType::Health) {
                    player.applyHealthPotion();
                }
                else {
                    player.applyStaminaPotion();
                }
                break;
            }
        }
    }

    if (pcx == maze.exit.first && pcy == maze.exit.second) {
        if (player.hasKey) {
            soundManager.playGateOpen();
            gameOver = true; win = true;
            message.setString("YOU WIN");
            message.setFillColor(sf::Color::Green);
        }
        else {
            showLockedMessage = true;
            lockedDoorMessage.setString("Door is locked! Find the key.");
            lockedMessageClock.restart();
        }
    }
}

void Game::updateHUD() {
    for (int i = 0; i < player.MAX_MAX_HP; ++i) {
        heartSprites[i].setTexture(i < player.hp ? assets.heartFull : assets.heartEmpty);
        if (i >= player.INITIAL_MAX_HP) {
            heartSprites[i].setColor(i < player.hp ? sf::Color(255, 255, 255, 255) : sf::Color(255, 255, 255, 100));
        }
    }

    float staminaPercent = player.stamina / player.maxStamina;
    staminaBarFg.setSize(sf::Vector2f(STAMINA_BAR_WIDTH * staminaPercent, STAMINA_BAR_HEIGHT));

    if (player.staminaRegenBonus > 0) {
        staminaBarFg.setFillColor(sf::Color(100, 255, 255));
    }
    else {
        staminaBarFg.setFillColor(sf::Color::Yellow);
    }
}

void Game::render() {
    window.clear(sf::Color::Black);

    window.setView(gameView);

    drawMaze();

    if (!player.hasKey) {
        keySprite.setPosition(maze.key.first * cellSize, maze.key.second * cellSize);
        window.draw(keySprite);
    }

    float totalTimeElapsed = deltaClock.getElapsedTime().asSeconds();

    for (auto& potion : potions) {
        if (potion.isActive) {
            const float waveSpeed = 4.0f;
            const float waveAmplitude = 2.5f;
            float offsetY = std::sin((potion.x + potion.y) + totalTimeElapsed * waveSpeed) * waveAmplitude;

            potion.sprite.setPosition(potion.x, potion.y + offsetY);
            window.draw(potion.sprite);
        }
    }

    for (auto& enemy : enemies) {
        window.draw(enemy.sprite);
    }

    if (player.isVisible) {
        window.draw(player.sprite);
    }

    if (showLockedMessage) {
        sf::FloatRect rect = lockedDoorMessage.getLocalBounds();
        lockedDoorMessage.setOrigin(rect.width / 2.f, rect.height / 2.f);
        lockedDoorMessage.setPosition(player.x + cellSize / 2, player.y - 20);
        window.draw(lockedDoorMessage);
    }

    window.setView(hudView);
    drawHUD();

    if (damageOverlay.getFillColor().a > 0) {
        window.draw(damageOverlay);
    }

    window.display();
}

void Game::drawMaze() {
    // Giữ nguyên (khá dài)
    // ... (copy/paste from original)
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            if (maze.walls[y][x]) {
                bool useWall2 = (y + 1 < H && !maze.walls[y + 1][x]); // Sửa logic: tường 2 chỉ khi bên dưới là sàn
                if (useWall2) {
                    wall2Sprite.setPosition(x * cellSize, y * cellSize);
                    window.draw(wallSprite);
                }
                else {
                    wallSprite.setPosition(x * cellSize, y * cellSize);
                    window.draw(wall2Sprite);
                }
            }
            else {
                groundSprite.setPosition(x * cellSize, y * cellSize);
                window.draw(groundSprite);

                if (maze.traps[y][x] == 1) {
                    redTrapSprite.setTextureRect(sf::IntRect(redTrapAnimFrame * redTrapSpriteSize, 0, redTrapSpriteSize, redTrapSpriteSize));
                    redTrapSprite.setPosition(x * cellSize, y * cellSize);
                    window.draw(redTrapSprite);
                }
                else if (maze.traps[y][x] == 2) {
                    yellowTrapSprite.setTextureRect(sf::IntRect(yellowTrapAnimFrame * yellowTrapSpriteSizeW, 0, yellowTrapSpriteSizeW, yellowTrapSpriteSizeH));
                    yellowTrapSprite.setPosition(x * cellSize, y * cellSize);
                    window.draw(yellowTrapSprite);

                }
            }

            if (x == maze.exit.first && y == maze.exit.second) {
                if (player.hasKey) {
                    exitDoorSprite.setTextureRect(sf::IntRect(0, 16, 16, 16)); // Cửa mở
                }
                else {
                    exitDoorSprite.setTextureRect(sf::IntRect(0, 0, 16, 16)); // Cửa đóng
                }
                float xOffset = (cellSize - 16 * exitDoorSprite.getScale().x) / 2.0f;
                float yOffset = (cellSize - 16 * exitDoorSprite.getScale().y) / 2.0f;
                exitDoorSprite.setPosition(x * cellSize + xOffset, y * cellSize + yOffset);
                window.draw(exitDoorSprite);
            }
        }
    }
}

void Game::drawHUD() {
    for (int i = 0; i < player.MAX_MAX_HP; ++i) {
        window.draw(heartSprites[i]);
    }

    window.draw(staminaIconSprite);
    window.draw(staminaBarBg);
    window.draw(staminaBarFg);

    window.draw(hudKeyIconSprite);
    window.draw(hudColon);
    if (player.hasKey) {
        window.draw(hudKeyVSprite);
    }
    else {
        window.draw(hudKeyXSprite);
    }

    window.draw(hudLevelText);
}
