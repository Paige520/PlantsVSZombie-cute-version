#include "GameScene.h"
#include "audio/include/AudioEngine.h"

USING_NS_CC;
using namespace cocos2d::experimental;

void GameScene::initAudioSystem() {
    musicEnabled = true;
    musicVolume = 1.0f;
    soundEffectVolume = 1.0f;
    
    // 创建音频配置文件
    audioProfile = new (std::nothrow) AudioProfile();
    audioProfile->name = "BackgroundMusic";
    audioProfile->maxInstances = 1;
    audioProfile->minDelay = 0.0f;
    
    // 播放背景音乐
    playBackgroundMusic();
}

void GameScene::playBackgroundMusic() {
    if (!musicEnabled) return;

    // 播放游戏背景音乐
    // 音乐文件路径可以根据实际情况修改
    std::string musicFile = "daytime.mp3"; 
    auto fileUtils = FileUtils::getInstance();
  
    // 播放音乐，循环播放
    AudioEngine::play2d(musicFile, true, musicVolume, audioProfile);
}

void GameScene::pauseBackgroundMusic() {
    // 暂停背景音乐
    AudioEngine::pauseAll();
    CCLOG("背景音乐已暂停");
}

void GameScene::resumeBackgroundMusic() {
    if (!musicEnabled) return;

    // 恢复背景音乐
    AudioEngine::resumeAll();
    CCLOG("背景音乐已恢复");
}

void GameScene::stopBackgroundMusic() {
    // 停止背景音乐
    AudioEngine::stopAll();
    CCLOG("背景音乐已停止");
}

void GameScene::setMusicVolume(float volume) {
    musicVolume = volume;
    if (musicVolume < 0.0f) musicVolume = 0.0f;
    if (musicVolume > 1.0f) musicVolume = 1.0f;

    // 设置所有音频的音量
    AudioEngine::setVolume(AudioEngine::INVALID_AUDIO_ID, musicVolume);
}

bool GameScene::isMusicPlaying() {
    // 获取当前正在播放的音频数量
    auto audioID = AudioEngine::getPlayingAudioCount();
    return audioID > 0;
}

// 播放音效
void GameScene::playSoundEffect(const std::string& soundFile) {
    if (!musicEnabled) return; // 假设音乐开关也控制音效
    
    // 创建音效音频配置
    AudioProfile* effectProfile = new (std::nothrow) AudioProfile();
    effectProfile->name = "SoundEffect";
    effectProfile->maxInstances = 5;
    effectProfile->minDelay = 0.1f;
    
    // 播放音效，不循环
    AudioEngine::play2d(soundFile, false, soundEffectVolume, effectProfile);
    
    // 释放配置文件
    CC_SAFE_DELETE(effectProfile);
}

// 设置音效音量
void GameScene::setSoundEffectVolume(float volume) {
    soundEffectVolume = volume;
    if (soundEffectVolume < 0.0f) soundEffectVolume = 0.0f;
    if (soundEffectVolume > 1.0f) soundEffectVolume = 1.0f;
}