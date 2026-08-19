#include "SoundSystem.h"
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

namespace {
constexpr const char* MUSIC_ALIAS = "tower_defense_music";
}

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
#ifdef _WIN32
    char command[512];
    std::snprintf(command, sizeof(command), "open \"%s\" type mpegvideo alias %s", filePath, MUSIC_ALIAS);
    MCIERROR error = mciSendStringA(command, nullptr, 0, nullptr);
    musicLoaded = (error == 0);
    if (!musicLoaded) {
        std::cout << "Music loading failed: " << filePath << std::endl;
    }
    return musicLoaded;
#else
    std::cout << "MP3 music is only supported on Windows in this build: " << filePath << std::endl;
    return false;
#endif
}

void SoundSystem::playMusic(bool loop) {
#ifdef _WIN32
    if (!musicLoaded || !soundEnabled || muted) return;
    const char* mode = loop ? " repeat" : "";
    char command[128];
    std::snprintf(command, sizeof(command), "play %s from 0%s", MUSIC_ALIAS, mode);
    mciSendStringA(command, nullptr, 0, nullptr);
#else
    (void)loop;
#endif
}

void SoundSystem::stopMusic() {
#ifdef _WIN32
    if (musicLoaded) {
        char command[128];
        std::snprintf(command, sizeof(command), "stop %s", MUSIC_ALIAS);
        mciSendStringA(command, nullptr, 0, nullptr);
    }
#endif
}

void SoundSystem::setMasterVolume(float volume) {
    masterVolume = (volume < 0.0f) ? 0.0f : (volume > 1.0f) ? 1.0f : volume;
}

void SoundSystem::setMuted(bool muted) {
    soundEnabled = !muted;
}

void SoundSystem::cleanup() {
#ifdef _WIN32
    if (musicLoaded) {
        char command[128];
        std::snprintf(command, sizeof(command), "close %s", MUSIC_ALIAS);
        mciSendStringA(command, nullptr, 0, nullptr);
        musicLoaded = false;
    }
#endif
}
