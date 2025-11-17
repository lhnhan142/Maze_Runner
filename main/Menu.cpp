#include "Menu.h"

int showMenu(sf::RenderWindow& window, sf::Font& font,
    const std::string& title, sf::Color titleColor,
    const std::vector<std::string>& options,
    const sf::Texture& bgTexture,
    SoundManager& soundManager)
{
    int selected = 0;
    sf::View originalView = window.getView();
    sf::View menuView(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
    window.setView(menuView);

    soundManager.startMainMenuMusic();

    sf::Sprite bgSprite(bgTexture);

    sf::Vector2u windowSize = window.getSize();
    sf::Vector2u textureSize = bgTexture.getSize();
    bgSprite.setScale(
        (float)windowSize.x / textureSize.x,
        (float)windowSize.y / textureSize.y
    );
    bgSprite.setPosition(0, 0);

    sf::Text titleText(title, font, 70);
    titleText.setFillColor(titleColor);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin(titleBounds.left + titleBounds.width / 2.0f,
        titleBounds.top + titleBounds.height / 2.0f);
    titleText.setPosition(window.getSize().x / 2.0f, 150.f);

    std::vector<sf::Text> optionTexts(options.size());
    for (int i = 0; i < options.size(); i++) {
        optionTexts[i].setFont(font);
        optionTexts[i].setString(options[i]);
        optionTexts[i].setCharacterSize(50);
        sf::FloatRect textBounds = optionTexts[i].getLocalBounds();
        optionTexts[i].setOrigin(textBounds.left + textBounds.width / 2.0f,
            textBounds.top + textBounds.height / 2.0f);
        optionTexts[i].setPosition(window.getSize().x / 2.0f, 350 + i * 70);
    }

    int previousSelected = selected;

    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) {
                soundManager.stopMainMenuMusic();
                return -1;
            }
            else if (e.type == sf::Event::KeyPressed) {
                if (e.key.code == sf::Keyboard::Up) {
                    selected = (selected + options.size() - 1) % options.size();
                }
                else if (e.key.code == sf::Keyboard::Down) {
                    selected = (selected + 1) % options.size();
                }
                else if (e.key.code == sf::Keyboard::Enter) {
                    soundManager.playClick();
                    soundManager.stopMainMenuMusic();
                    window.setView(originalView);
                    return selected;
                }
            }
            else if (e.type == sf::Event::MouseButtonPressed) {
                if (e.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                    for (int i = 0; i < optionTexts.size(); ++i) {
                        if (optionTexts[i].getGlobalBounds().contains(mousePos)) {
                            soundManager.playClick();
                            soundManager.stopMainMenuMusic();
                            window.setView(originalView);
                            return i;
                        }
                    }
                }
            }
            else if (e.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (int i = 0; i < optionTexts.size(); ++i) {
                    if (optionTexts[i].getGlobalBounds().contains(mousePos)) {
                        selected = i;
                        break;
                    }
                }
            }
        }

        if (selected != previousSelected) {
            soundManager.playClick();
            previousSelected = selected;
        }

        window.clear();
        window.draw(bgSprite);
        window.draw(titleText);

        for (int i = 0; i < optionTexts.size(); i++) {
            optionTexts[i].setFillColor(i == selected ? sf::Color::Yellow : sf::Color::White);
            window.draw(optionTexts[i]);
        }

        window.display();
    }
    soundManager.stopMainMenuMusic();
    window.setView(originalView);
    return -1;
}