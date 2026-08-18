#include "SoundSystem.h"
#include <iostream>

SoundSystem::SoundSystem() {
    // Khởi tạo hệ thống âm thanh
    // SDL3 audio sẽ được khởi tạo ở RenderSystem
}

SoundSystem::~SoundSystem() {
    cleanup();
}

bool SoundSystem::loadSound(const std::string& id, const char* filePath) {
    // TODO: Implement sound loading với SDL3_mixer hoặc SDL3 audio
    // Hiện tại là stub để tránh lỗi compile
    std::cout << "Sound loading not yet implemented: " << id << " from " << filePath << std::endl;
    return true;
}

void SoundSystem::playSound(const std::string& id, float volume) {
    if (!soundEnabled || masterVolume <= 0.0f) return;
    // TODO: Implement playback
}

bool SoundSystem::loadMusic(const char* filePath) {
    // TODO: Implement music loading
    return true;
}

void SoundSystem::playMusic(bool loop) {
    // TODO: Implement music playback
}

void SoundSystem::stopMusic() {
    // TODO: Implement music stop
}

void SoundSystem::setMasterVolume(float volume) {
    masterVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
}

void SoundSystem::setMuted(bool muted) {
    soundEnabled = !muted;
}

void SoundSystem::cleanup() {
    // TODO: Cleanup audio resources
}
