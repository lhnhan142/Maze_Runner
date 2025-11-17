#include "SoundManager.h"

bool SoundManager::loadSounds() {
    bool success = true;

    if (!clickBuffer.loadFromFile("../Sound/click.wav")) { std::cout << "Failed to load click.wav\n"; success = false; }
    else clickSound.setBuffer(clickBuffer);

    if (!gateOpenBuffer.loadFromFile("../Sound/gate_open.wav")) { std::cout << "Failed to load gate_open.wav\n"; success = false; }
    else gateOpenSound.setBuffer(gateOpenBuffer);

    if (!keyPickupBuffer.loadFromFile("../Sound/key_pickup.wav")) { std::cout << "Failed to load key_pickup.wav\n"; success = false; }
    else keyPickupSound.setBuffer(keyPickupBuffer);

    if (!healBuffer.loadFromFile("../Sound/heal.wav")) { std::cout << "Failed to load heal.wav\n"; success = false; }
    else healSound.setBuffer(healBuffer);

    if (!walkingBuffer.loadFromFile("../Sound/skeleton.wav")) { std::cout << "Failed to load walking.wav\n"; success = false; }
    else {
        walkingSound.setBuffer(walkingBuffer);
        walkingSound.setLoop(true);
        walkingSound.setVolume(20.f);
    }

    if (!goblinLaughBuffer.loadFromFile("../Sound/goblin_laugh.wav")) { std::cout << "Failed to load goblin_laugh.wav\n"; success = false; }
    else goblinLaughSound.setBuffer(goblinLaughBuffer);

    if (!tindeckBuffer.loadFromFile("../Sound/tindeck.wav")) { std::cout << "Failed to load tindeck.wav\n"; success = false; }
    else tindeckSound.setBuffer(tindeckBuffer);

    if (!damageBuffer.loadFromFile("../Sound/grunt.wav")) { std::cout << "Failed to load grunt.wav (damage sound)\n"; success = false; }
    else damageSound.setBuffer(damageBuffer);

    if (!mainMenuMusic.openFromFile("../Sound/main_menu.wav")) { std::cout << "Failed to load main_menu.wav\n"; success = false; }
    else {
        mainMenuMusic.setLoop(true);
        mainMenuMusic.setVolume(50.f);
    }

    if (!bgThemeMusic.openFromFile("../Sound/bg_theme.wav")) { std::cout << "Failed to load bg_theme.wav\n"; success = false; }
    else {
        bgThemeMusic.setLoop(true);
        bgThemeMusic.setVolume(30.f);
    }

    if (!caveAmbienceMusic.openFromFile("../Sound/cave_ambience.wav")) { std::cout << "Failed to load cave_ambience.wav\n"; success = false; }
    else {
        caveAmbienceMusic.setLoop(true);
        caveAmbienceMusic.setVolume(10.f);
    }

    return success;
}

void SoundManager::playClick() { clickSound.play(); }
void SoundManager::playKeyPickup() { keyPickupSound.play(); }
void SoundManager::playHeal() { healSound.play(); }
void SoundManager::playDamage() { damageSound.play(); }
void SoundManager::playGateOpen() { gateOpenSound.play(); }
void SoundManager::playGoblinLaugh() { goblinLaughSound.play(); }
void SoundManager::playTindeck() { tindeckSound.play(); }

void SoundManager::startWalking() { if (walkingSound.getStatus() != sf::Sound::Playing) walkingSound.play(); }
void SoundManager::stopWalking() { walkingSound.stop(); }

void SoundManager::startMainMenuMusic() { mainMenuMusic.play(); }
void SoundManager::stopMainMenuMusic() { mainMenuMusic.stop(); }
void SoundManager::startGameMusic() {
    bgThemeMusic.play();
    caveAmbienceMusic.play();
}
void SoundManager::stopGameMusic() {
    bgThemeMusic.stop();
    caveAmbienceMusic.stop();
}