#include "GameScene.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;
using namespace cocos2d::experimental;

void GameScene::initAudioSystem() {
    musicEnabled = true;
    musicVolume = 1.0f;
    soundEffectVolume = 1.0f;
    
    // ¥¥Ω®“Ù∆µ≈‰÷√Œƒº˛
    audioProfile = new (std::nothrow) AudioProfile();
    audioProfile->name = "BackgroundMusic";
    audioProfile->maxInstances = 1;
    audioProfile->minDelay = 0.0f;
    
    // ≤•∑≈±≥æ∞“Ù¿÷
    playBackgroundMusic();
}

void GameScene::playBackgroundMusic() {
    if (!musicEnabled) return;

    // ≤•∑≈”Œœ∑±≥æ∞“Ù¿÷
    std::string musicFile = "daytime.mp3"; 
    auto fileUtils = FileUtils::getInstance();
  
    // ≤•∑≈“Ù¿÷£¨—≠ª∑≤•∑≈
    AudioEngine::play2d(musicFile, true, musicVolume, audioProfile);
}

void GameScene::pauseBackgroundMusic() {
    // ‘›Õ£±≥æ∞“Ù¿÷
    AudioEngine::pauseAll();
    CCLOG("±≥æ∞“Ù¿÷“—‘›Õ£");
}

void GameScene::resumeBackgroundMusic() {
    if (!musicEnabled) return;

    // ª÷∏¥±≥æ∞“Ù¿÷
    AudioEngine::resumeAll();
    CCLOG("±≥æ∞“Ù¿÷“—ª÷∏¥");
}

void GameScene::stopBackgroundMusic() {
    // Õ£÷π±≥æ∞“Ù¿÷
    AudioEngine::stopAll();
    CCLOG("±≥æ∞“Ù¿÷“—Õ£÷π");
}

void GameScene::setMusicVolume(float volume) {
    musicVolume = volume;
    if (musicVolume < 0.0f) musicVolume = 0.0f;
    if (musicVolume > 1.0f) musicVolume = 1.0f;

    // …Ë÷√À˘”–“Ù∆µµƒ“Ù¡ø
    AudioEngine::setVolume(AudioEngine::INVALID_AUDIO_ID, musicVolume);
}

bool GameScene::isMusicPlaying() {
    // ªÒ»°µ±«∞’˝‘⁄≤•∑≈µƒ“Ù∆µ ˝¡ø
    auto audioID = AudioEngine::getPlayingAudioCount();
    return audioID > 0;
}

// ≤•∑≈“Ù–ß
void GameScene::playSoundEffect(const std::string& soundFile) {
    if (!musicEnabled) return; // ºŸ…Ë“Ù¿÷ø™πÿ“≤øÿ÷∆“Ù–ß
    
    // ¥¥Ω®“Ù–ß“Ù∆µ≈‰÷√
    AudioProfile* effectProfile = new (std::nothrow) AudioProfile();
    effectProfile->name = "SoundEffect";
    effectProfile->maxInstances = 5;
    effectProfile->minDelay = 0.1f;
    
    // ≤•∑≈“Ù–ß£¨≤ª—≠ª∑
    AudioEngine::play2d(soundFile, false, soundEffectVolume, effectProfile);
    
    //  Õ∑≈≈‰÷√Œƒº˛
    CC_SAFE_DELETE(effectProfile);
}

// …Ë÷√“Ù–ß“Ù¡ø
void GameScene::setSoundEffectVolume(float volume) {
    soundEffectVolume = volume;
    if (soundEffectVolume < 0.0f) soundEffectVolume = 0.0f;
    if (soundEffectVolume > 1.0f) soundEffectVolume = 1.0f;
}