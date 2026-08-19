#pragma once
#include <SDL3/SDL.h>
#include <unordered_map>
#include <string>

class SoundSystem {
private:
    std::unordered_map<std::string, SDL_AudioStream*> soundEffects;
    SDL_AudioStream* backgroundMusic = nullptr;
    float masterVolume = 0.7f;
    bool soundEnabled = true;
    bool muted = false;
    bool musicLoaded = false;
    
public:
    SoundSystem();
    ~SoundSystem();
    
    // Tải và phát âm thanh
    bool loadSound(const std::string& id, const char* filePath);
    void playSound(const std::string& id, float volume = 1.0f);
    
    // Nhạc nền
    bool loadMusic(const char* filePath);
    void playMusic(bool loop = true);
    void stopMusic();
    
    // Điều khiển âm lượng
    void setMasterVolume(float volume);  // 0.0 - 1.0
    void setMuted(bool muted);
    
    // Dọn dẹp
    void cleanup();
};
