#pragma once
#include <SDL3/SDL.h>
#include "GameData.h"
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>
#include <string>
#include "AssetManager.h"
class RenderSystem {
private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    AssetManager assets;
    std::unordered_map<std::string, SDL_Texture*> textCache;
    void renderText(const std::string& text, float x, float y, SDL_Color color, bool isStatic = false);
public:
    bool init();
    void draw(const GameData& data);
    void cleanup();
};