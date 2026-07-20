#pragma once
#include <SDL3/SDL.h>
#include "GameData.h"

class RenderSystem {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Texture* backgroundTexture = nullptr;
    SDL_Texture* healthBarTextures[7] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};

public:
    bool init();
    void draw(const GameData& data);
    void cleanup();
};