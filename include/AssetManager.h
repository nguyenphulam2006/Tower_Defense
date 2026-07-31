#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>
#include <string>
#include <iostream>

class AssetManager {
private:
    std::unordered_map<std::string, SDL_Texture*> textures;
    std::unordered_map<std::string, TTF_Font*> fonts;

public:
    ~AssetManager();

    // Tải và lưu trữ Texture
    bool loadTexture(SDL_Renderer* renderer, const std::string& id, const char* primaryPath, const char* fallbackPath = nullptr);
    
    // Lấy Texture đã tải
    SDL_Texture* getTexture(const std::string& id);

    // Tải và lưu trữ Font
    bool loadFont(const std::string& id, const char* primaryPath, int fontSize, const char* fallbackPath = nullptr);
    
    // Lấy Font đã tải
    TTF_Font* getFont(const std::string& id);

    // Xóa toàn bộ tài nguyên
    void clear();
};