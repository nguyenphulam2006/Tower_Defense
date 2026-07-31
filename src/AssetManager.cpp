#include "AssetManager.h"

AssetManager::~AssetManager() {
    clear();
}

bool AssetManager::loadTexture(SDL_Renderer* renderer, const std::string& id, const char* primaryPath, const char* fallbackPath) {
    if (textures.find(id) != textures.end()) {
        return true; // Đã load rồi
    }

    SDL_Surface* surface = SDL_LoadPNG(primaryPath);
    if (!surface && fallbackPath != nullptr) {
        surface = SDL_LoadPNG(fallbackPath);
    }

    if (!surface) {
        std::cout << "AssetManager - Loi load PNG: " << primaryPath << " | " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);

    if (texture) {
        textures[id] = texture;
        return true;
    }
    return false;
}

SDL_Texture* AssetManager::getTexture(const std::string& id) {
    auto it = textures.find(id);
    if (it != textures.end()) {
        return it->second;
    }
    return nullptr;
}

bool AssetManager::loadFont(const std::string& id, const char* primaryPath, int fontSize, const char* fallbackPath) {
    TTF_Font* font = TTF_OpenFont(primaryPath, fontSize);
    if (!font && fallbackPath != nullptr) {
        font = TTF_OpenFont(fallbackPath, fontSize);
    }
    
    if (font) {
        fonts[id] = font;
        return true;
    }
    return false;
}

TTF_Font* AssetManager::getFont(const std::string& id) {
    auto it = fonts.find(id);
    if (it != fonts.end()) {
        return it->second;
    }
    return nullptr;
}

void AssetManager::clear() {
    for (auto& pair : textures) {
        if (pair.second) SDL_DestroyTexture(pair.second);
    }
    textures.clear();

    for (auto& pair : fonts) {
        if (pair.second) TTF_CloseFont(pair.second);
    }
    fonts.clear();
}