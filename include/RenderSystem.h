#pragma once
#include <SDL3/SDL.h>
#include "GameData.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>
#include <string>
class RenderSystem {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
   SDL_Texture* grassTexture = nullptr; 
    SDL_Texture* brickTexture = nullptr;
    SDL_Texture* heartTexture = nullptr;
    SDL_Texture* heartAnimatedTexture = nullptr;
    SDL_Texture* waterTexture = nullptr;
    SDL_Texture* lavaTexture = nullptr;
    SDL_Texture* enemyAnimatedTexture = nullptr;
    SDL_Texture* towerArcherTexture = nullptr;
    SDL_Texture* arrowProjectileTexture = nullptr;
    SDL_Texture* bgMainMenuTexture = nullptr;
    SDL_Texture* panelTexture = nullptr;
    SDL_Texture* buttonGreenTexture = nullptr;
    TTF_Font* font = nullptr;
    std::unordered_map<std::string, SDL_Texture*> textCache;
    void renderText(const std::string& text, float x, float y, SDL_Color color, bool isStatic = false);
public:
    bool init();
    void draw(const GameData& data);
    void cleanup();
};