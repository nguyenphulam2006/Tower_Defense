#include "RenderSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

namespace {
struct ColorSwatch {
    Uint8 r = 0;
    Uint8 g = 0;
    Uint8 b = 0;
    Uint8 a = 255;
};

struct TilePalette {
    ColorSwatch grass {60, 105, 60, 255};
    ColorSwatch path {120, 100, 70, 255};
    ColorSwatch base {0, 120, 255, 255};
    ColorSwatch grid {55, 55, 55, 255};
    ColorSwatch enemy {255, 50, 50, 255};
    ColorSwatch towerBasic {50, 255, 50, 255};
    ColorSwatch towerSniper {255, 180, 40, 255};
    ColorSwatch towerSplash {180, 80, 255, 255};
    ColorSwatch projectileBasic {255, 220, 0, 255};
    ColorSwatch projectileSniper {255, 255, 255, 255};
    ColorSwatch projectileSplash {180, 80, 255, 255};
    ColorSwatch buttonRetry {200, 60, 60, 255};
    ColorSwatch buttonRetryHover {230, 90, 90, 255};
    ColorSwatch overlay {0, 0, 0, 160};
};

TilePalette palette;

ColorSwatch parseColorLine(const std::string& line) {
    std::istringstream stream(line);
    std::string key;
    int r = 0;
    int g = 0;
    int b = 0;
    int a = 255;

    if (!(stream >> key >> r >> g >> b >> a)) {
        return {};
    }

    return {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
}

void setColor(SDL_Renderer* renderer, const ColorSwatch& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
}

SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* primaryPath, const char* fallbackPath = nullptr) {
    SDL_Surface* surface = SDL_LoadPNG(primaryPath);
    if (!surface && fallbackPath != nullptr) {
        surface = SDL_LoadPNG(fallbackPath);
    }

    if (!surface) {
        std::cout << "Khong load duoc asset PNG: " << primaryPath << " | " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_DestroySurface(surface);
    if (!texture) {
        std::cout << "Khong tao duoc texture: " << primaryPath << " | " << SDL_GetError() << std::endl;
    }

    return texture;
}

void setTowerColor(SDL_Renderer* renderer, TowerType type) {
    switch (type) {
        case TowerType::Basic:
            SDL_SetRenderDrawColor(renderer, 50, 255, 50, 255);
            break;
        case TowerType::Sniper:
            SDL_SetRenderDrawColor(renderer, 255, 180, 40, 255);
            break;
        case TowerType::Splash:
            SDL_SetRenderDrawColor(renderer, 180, 80, 255, 255);
            break;
    }
}

void setProjectileColor(SDL_Renderer* renderer, TowerType type) {
    switch (type) {
        case TowerType::Basic:
            SDL_SetRenderDrawColor(renderer, 255, 220, 0, 255);
            break;
        case TowerType::Sniper:
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            break;
        case TowerType::Splash:
            SDL_SetRenderDrawColor(renderer, 180, 80, 255, 255);
            break;
    }
}

const char* towerName(TowerType type) {
    switch (type) {
        case TowerType::Basic:
            return "Basic";
        case TowerType::Sniper:
            return "Sniper";
        case TowerType::Splash:
            return "Splash";
    }

    return "Basic";
}

bool loadPaletteFromFile(TilePalette& palette) {
    std::ifstream file("assets/tileset.txt");
    if (!file.is_open()) {
        file.open("../assets/tileset.txt");
    }

    if (!file.is_open()) {
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream stream(line);
        std::string key;
        int r = 0;
        int g = 0;
        int b = 0;
        int a = 255;
        if (!(stream >> key >> r >> g >> b >> a)) {
            continue;
        }

        ColorSwatch color {static_cast<Uint8>(r), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a)};
        if (key == "grass") palette.grass = color;
        else if (key == "path") palette.path = color;
        else if (key == "base") palette.base = color;
        else if (key == "grid") palette.grid = color;
        else if (key == "enemy") palette.enemy = color;
        else if (key == "tower_basic") palette.towerBasic = color;
        else if (key == "tower_sniper") palette.towerSniper = color;
        else if (key == "tower_splash") palette.towerSplash = color;
        else if (key == "projectile_basic") palette.projectileBasic = color;
        else if (key == "projectile_sniper") palette.projectileSniper = color;
        else if (key == "projectile_splash") palette.projectileSplash = color;
        else if (key == "button_retry") palette.buttonRetry = color;
        else if (key == "button_retry_hover") palette.buttonRetryHover = color;
        else if (key == "overlay") palette.overlay = color;
    }

    return true;
}
}

bool RenderSystem::init() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
    std::cout << "SDL_Init loi: " << SDL_GetError() << std::endl;
    return false;
}

    window = SDL_CreateWindow("Arknights - SDL3 Modular", MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) return false;

    loadPaletteFromFile(palette);

    backgroundTexture = loadTexture(
        renderer,
        "assets/craftpix-net-305231-free-tower-defense-2d-vector-tileset/PNG/game_background_4/game_background_4.png",
        "../assets/craftpix-net-305231-free-tower-defense-2d-vector-tileset/PNG/game_background_4/game_background_4.png"
    );

    for (int i = 0; i < 7; ++i) {
        std::string index = std::to_string(i + 1);
        std::string primary = "assets/craftpix-437811-free-monster-enemy-game-sprites/health_bar/health_bar-0" + index + ".png";
        std::string fallback = "../assets/craftpix-437811-free-monster-enemy-game-sprites/health_bar/health_bar-0" + index + ".png";
        healthBarTextures[i] = loadTexture(renderer, primary.c_str(), fallback.c_str());
    }

    return true;
}

void RenderSystem::draw(const GameData& data) {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);

    if (backgroundTexture != nullptr) {
        SDL_FRect backgroundDst = {0.0f, 0.0f, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE)};
        SDL_RenderTexture(renderer, backgroundTexture, nullptr, &backgroundDst);
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            TileType tile = data.tileMap[y][x];
            if (tile == TileType::Grass) {
                setColor(renderer, {palette.grass.r, palette.grass.g, palette.grass.b, 90});
            } else if (tile == TileType::Path) {
                setColor(renderer, {palette.path.r, palette.path.g, palette.path.b, 150});
            } else {
                setColor(renderer, {palette.base.r, palette.base.g, palette.base.b, 180});
            }
            SDL_FRect cell = { (float)x * TILE_SIZE, (float)y * TILE_SIZE, (float)TILE_SIZE - 1.0f, (float)TILE_SIZE - 1.0f };
            SDL_RenderFillRect(renderer, &cell);
        }
    }

    if (data.hoveredGridX >= 0 && data.hoveredGridX < MAP_WIDTH && data.hoveredGridY >= 0 && data.hoveredGridY < MAP_HEIGHT) {
        if (data.hoveredGridBuildable) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 160);
        } else {
            SDL_SetRenderDrawColor(renderer, 255, 80, 80, 160);
        }
        SDL_FRect hoverRect = { (float)data.hoveredGridX * TILE_SIZE + 2.0f, (float)data.hoveredGridY * TILE_SIZE + 2.0f, (float)TILE_SIZE - 4.0f, (float)TILE_SIZE - 4.0f };
        SDL_RenderRect(renderer, &hoverRect);
    }

    // Vẽ Quái vật
    for (const auto& enemy : data.enemies) {
        if (!enemy.active) {
            continue;
        }

        Position enemyPos = data.enemyPath[enemy.currentStep];
        setColor(renderer, palette.enemy);
        SDL_FRect enemyRect = { 
            (float)enemyPos.x * TILE_SIZE + 16, 
            (float)enemyPos.y * TILE_SIZE + 16, 
            32.0f, 32.0f 
        };
        SDL_RenderFillRect(renderer, &enemyRect);
    }

    // Vẽ Operators (Màu xanh lá)
    for (const auto& op : data.operators) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 220);
        SDL_FRect opBorder = { 
            (float)op.pos.x * TILE_SIZE + 4, 
            (float)op.pos.y * TILE_SIZE + 4, 
            56.0f, 56.0f 
        };
        SDL_RenderRect(renderer, &opBorder);

        if (op.type == TowerType::Basic) setColor(renderer, palette.towerBasic);
        else if (op.type == TowerType::Sniper) setColor(renderer, palette.towerSniper);
        else setColor(renderer, palette.towerSplash);
        // Vẽ hình vuông nhỏ hơn ô một chút
        SDL_FRect opRect = { 
            (float)op.pos.x * TILE_SIZE + 8, 
            (float)op.pos.y * TILE_SIZE + 8, 
            48.0f, 48.0f 
        };
        SDL_RenderFillRect(renderer, &opRect);
    }

    // Vẽ đạn
    for (const auto& projectile : data.projectiles) {
        if (projectile.sourceType == TowerType::Basic) setColor(renderer, palette.projectileBasic);
        else if (projectile.sourceType == TowerType::Sniper) setColor(renderer, palette.projectileSniper);
        else setColor(renderer, palette.projectileSplash);
        SDL_FRect bullet = { projectile.x - 4.0f, projectile.y - 4.0f, 8.0f, 8.0f };
        SDL_RenderFillRect(renderer, &bullet);
    }

    if (data.gameOver) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        setColor(renderer, palette.overlay);
        SDL_FRect overlay = { 0.0f, 0.0f, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE) };
        SDL_RenderFillRect(renderer, &overlay);

        bool hover = false;
        float mouseX = 0.0f;
        float mouseY = 0.0f;
        SDL_GetMouseState(&mouseX, &mouseY);
        hover = mouseX >= RETRY_BUTTON_X && mouseX < RETRY_BUTTON_X + RETRY_BUTTON_WIDTH && mouseY >= RETRY_BUTTON_Y && mouseY < RETRY_BUTTON_Y + RETRY_BUTTON_HEIGHT;

        setColor(renderer, hover ? palette.buttonRetryHover : palette.buttonRetry);
        SDL_FRect retryButton = { (float)RETRY_BUTTON_X, (float)RETRY_BUTTON_Y, (float)RETRY_BUTTON_WIDTH, (float)RETRY_BUTTON_HEIGHT };
        SDL_RenderFillRect(renderer, &retryButton);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_FRect retryBorder = { (float)RETRY_BUTTON_X, (float)RETRY_BUTTON_Y, (float)RETRY_BUTTON_WIDTH, (float)RETRY_BUTTON_HEIGHT };
        SDL_RenderRect(renderer, &retryBorder);
    }

    float hpRatio = data.baseHP / 3.0f;
    if (hpRatio < 0.0f) {
        hpRatio = 0.0f;
    }
    if (hpRatio > 1.0f) {
        hpRatio = 1.0f;
    }

    int hpIndex = (int)(hpRatio * 6.0f + 0.5f);
    if (hpIndex < 0) hpIndex = 0;
    if (hpIndex > 6) hpIndex = 6;
    SDL_Texture* hpTexture = healthBarTextures[hpIndex];
    if (hpTexture != nullptr) {
        SDL_FRect hpBarDst = { 16.0f, 16.0f, 280.0f, 56.0f };
        SDL_RenderTexture(renderer, hpTexture, nullptr, &hpBarDst);
    }

    std::string title = "Tower Defense | Wave: " + std::to_string(data.currentWave) +
        " | HP: " + std::to_string(data.baseHP) +
        " | Gold: " + std::to_string(data.gold) +
        " | Selected: " + towerName(data.selectedTowerType) +
        " | Towers: " + std::to_string((int)data.operators.size());
    if (data.gameOver) {
        title += " | GAME OVER - Click Retry or press R";
    } else if (data.hoveredGridX >= 0 && data.hoveredGridY >= 0) {
        title += " | Hover: (" + std::to_string(data.hoveredGridX) + "," + std::to_string(data.hoveredGridY) + ")";
        title += data.hoveredGridBuildable ? " placeable" : " blocked";
    }
    SDL_SetWindowTitle(window, title.c_str());

    SDL_RenderPresent(renderer);
}

void RenderSystem::cleanup() {
    for (SDL_Texture*& texture : healthBarTextures) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    SDL_DestroyTexture(backgroundTexture);
    backgroundTexture = nullptr;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}