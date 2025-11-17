#pragma once
#include <SFML/Audio.hpp>
#include <iostream>

// =================== STRUCT QUẢN LÝ ÂM THANH ===================
struct SoundManager {
    sf::SoundBuffer clickBuffer;
    sf::SoundBuffer keyPickupBuffer;
    sf::SoundBuffer healBuffer;
    sf::SoundBuffer walkingBuffer;
    sf::SoundBuffer goblinLaughBuffer;
    sf::SoundBuffer tindeckBuffer;
    sf::SoundBuffer damageBuffer;
    sf::SoundBuffer gateOpenBuffer;

    sf::Music mainMenuMusic;
    sf::Music bgThemeMusic;
    sf::Music caveAmbienceMusic;

    sf::Sound clickSound;
    sf::Sound keyPickupSound;
    sf::Sound healSound;
    sf::Sound walkingSound;
    sf::Sound goblinLaughSound;
    sf::Sound tindeckSound;
    sf::Sound damageSound;
    sf::Sound gateOpenSound;

    bool loadSounds();

    void playClick();
    void playKeyPickup();
    void playHeal();
    void playDamage();
    void playGateOpen();
    void playGoblinLaugh();
    void playTindeck();

    void startWalking();
    void stopWalking();

    void startMainMenuMusic();
    void stopMainMenuMusic();
    void startGameMusic();
    void stopGameMusic();
};