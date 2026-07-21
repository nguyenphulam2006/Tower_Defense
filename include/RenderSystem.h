#pragma once
#include <SDL3/SDL.h>
#include "GameData.h"

class RenderSystem {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
   SDL_Texture* grassTexture = nullptr; 
    SDL_Texture* brickTexture = nullptr;
    SDL_Texture* heartTexture = nullptr;
    SDL_Texture* heartAnimatedTexture = nullptr;
public:
    bool init();
    void draw(const GameData& data);
    void cleanup();
};