#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include "Game.h"
#include "Menu.h"
#include "AssetManager.h"
#include "SoundManager.h"
#include "GameTypes.h"

// =================== HÀM MAIN ===================
int main() {
    sf::RenderWindow window(sf::VideoMode(1080, 720), "Maze Game");
    window.setFramerateLimit(60);
    srand(time(NULL));

    sf::Font font;
    if (!font.loadFromFile("../font/arial.ttf")) {
        std::cout << "Place arial.ttf in folder\n";
        return 0;
    }

    AssetManager assetManager;
    if (!assetManager.loadAssets()) {
        std::cout << "Failed to load one or more assets!\n";
        std::cout << "Check file names are correct (goblin2.png, background.png, etc.).\n";
        return -1;
    }

    SoundManager soundManager;
    if (!soundManager.loadSounds()) {
        std::cout << "Failed to load one or more sound files. Continue without sound.\n";
    }

    std::vector<std::string> mainMenuOptions = { "New Game", "Exit" };

    while (window.isOpen()) {
        int choice = showMenu(window, font,
            "MAZE GAME", sf::Color::Cyan,
            mainMenuOptions,
            assetManager.background,
            soundManager);

        if (choice == 0) { // New Game
            int currentLevel = 1;
            GameState state = GameState::PlayAgain;

            while ((state == GameState::PlayAgain || state == GameState::NextLevel) && window.isOpen()) {
                if (state == GameState::NextLevel) {
                    currentLevel++;
                }

                // Logic của hàm playGame cũ được đưa vào đây
                Game game(window, font, assetManager, soundManager, currentLevel);
                state = game.run();
                // Đối tượng 'game' sẽ tự động bị hủy (gọi destructor) khi ra khỏi scope này
            }

            if (state == GameState::Exit) break;
        }
        else { // Exit
            break;
        }
    }
    return 0;
}