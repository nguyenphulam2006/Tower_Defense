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

    window = SDL_CreateWindow("Tower Defense - SDL3 Modular", MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) return false;

    loadPaletteFromFile(palette);
grassTexture = loadTexture(
        renderer,
        "assets/grass.jpg",
        "../assets/grass.jpg"
    );

    brickTexture = loadTexture(
        renderer,
        "assets/brick.jpg",
        "../assets/brick.jpg"
    );
    heartTexture = loadTexture(
        renderer,
        "assets/heart.png",
        "../assets/heart.png"
    );
    heartAnimatedTexture = loadTexture(
        renderer,
        "assets/heart_animated_1.png",
        "../assets/heart_animated_1.png"
    );
    return true;
}

void RenderSystem::draw(const GameData& data) {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            TileType tile = data.tileMap[y][x];
            SDL_FRect cell = { (float)x * TILE_SIZE, (float)y * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
            if (tile == TileType::Grass) {
                if (grassTexture != nullptr) {
                    SDL_RenderTexture(renderer, grassTexture, nullptr, &cell);
                } else {
                    setColor(renderer, {palette.grass.r, palette.grass.g, palette.grass.b, 255}); // 255 thay vì 90 để không trong suốt
                    SDL_RenderFillRect(renderer, &cell);
                }
            } else if (tile == TileType::Path || tile == TileType::Base) {
                if (brickTexture != nullptr) {
                    SDL_RenderTexture(renderer, brickTexture, nullptr, &cell);
                } else {
                    setColor(renderer, {palette.path.r, palette.path.g, palette.path.b, 255}); // 255 để không trong suốt
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
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
int maxHearts = 3;
    float heartSize = 40.0f;
    float heartGap = 10.0f;
    float startX = 16.0f;
    float startY = 16.0f;

    for (int i = 0; i < maxHearts; ++i) {
        SDL_FRect heartDst = { startX + i * (heartSize + heartGap), startY, heartSize, heartSize };
        
        if (i < data.baseHP) {
            // Còn máu -> Vẽ trái tim bình thường
            if (heartTexture != nullptr) {
                SDL_RenderTexture(renderer, heartTexture, nullptr, &heartDst);
            } else {
                setColor(renderer, {255, 50, 50, 255});
                SDL_RenderFillRect(renderer, &heartDst);
            }
        } else {
            // Mất máu -> Sử dụng heart_animated_1.png (5 frames)
            if (heartAnimatedTexture != nullptr) {
                int totalFrames = 5; 
                float texW, texH;
                SDL_GetTextureSize(heartAnimatedTexture, &texW, &texH);
                float frameW = texW / totalFrames;
                
                // Animation nhấp nháy hoặc hiển thị frame cuối
             Uint64 timeSinceLost = SDL_GetTicks() - data.lostHeartTime[i];
                int currentFrame = (int)(timeSinceLost / 150);
                
                if (currentFrame >= totalFrames) {
                    currentFrame = totalFrames - 1; // Giữ nguyên ở frame cuối cùng (trái tim vỡ)
                }
                
                SDL_FRect srcRect = { currentFrame * frameW, 0.0f, frameW, texH };
                
                SDL_FRect srcRect = { currentFrame * frameW, 0.0f, frameW, texH };
                SDL_RenderTexture(renderer, heartAnimatedTexture, &srcRect, &heartDst);
            } else {
                setColor(renderer, {100, 100, 100, 255});
                SDL_RenderFillRect(renderer, &heartDst);
            }
        }
    }

if (data.gameState != GameState::Playing) {
    // Vẽ màn mờ overlay
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    setColor(renderer, palette.overlay);
    SDL_FRect overlay = { 0.0f, 0.0f, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE) };
    SDL_RenderFillRect(renderer, &overlay);

    // Tính toán tọa độ nút bấm (dựa trên các biến const trong GameData.h)
    int btnX = (MAP_WIDTH * TILE_SIZE - MENU_BUTTON_WIDTH) / 2;
    int btnY = (MAP_HEIGHT * TILE_SIZE - MENU_BUTTON_HEIGHT) / 2;
    
    float mouseX = 0.0f, mouseY = 0.0f;
    SDL_GetMouseState(&mouseX, &mouseY);
    bool hover = mouseX >= btnX && mouseX < btnX + MENU_BUTTON_WIDTH && mouseY >= btnY && mouseY < btnY + MENU_BUTTON_HEIGHT;

    setColor(renderer, hover ? palette.buttonRetryHover : palette.buttonRetry);
    SDL_FRect menuButton = { (float)btnX, (float)btnY, (float)MENU_BUTTON_WIDTH, (float)MENU_BUTTON_HEIGHT };
    SDL_RenderFillRect(renderer, &menuButton);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderRect(renderer, &menuButton);

    std::string stateTitle = "Tower Defense | ";
    if (data.gameState == GameState::MainMenu) stateTitle += "MAIN MENU - Click Button to Start";
    else if (data.gameState == GameState::Paused) stateTitle += "PAUSED - Click Button or ESC to Resume";
    else if (data.gameState == GameState::GameOver) stateTitle += "GAME OVER - Click Button to Retry";
    
    SDL_SetWindowTitle(window, stateTitle.c_str());
} else {
    std::string title = "Tower Defense | Wave: " + std::to_string(data.currentWave) +
        " | HP: " + std::to_string(data.baseHP) +
        " | Gold: " + std::to_string(data.gold) +
        " | Selected: " + towerName(data.selectedTowerType);
    SDL_SetWindowTitle(window, title.c_str());
}
    SDL_RenderPresent(renderer);
}
void RenderSystem::cleanup() {
    if (heartAnimatedTexture != nullptr) {
        SDL_DestroyTexture(heartAnimatedTexture);
        heartAnimatedTexture = nullptr;
    }
    if (heartTexture != nullptr) {
        SDL_DestroyTexture(heartTexture);
        heartTexture = nullptr;
    }
    if (grassTexture != nullptr) {
        SDL_DestroyTexture(grassTexture);
        grassTexture = nullptr;
    }
    if (brickTexture != nullptr) {
        SDL_DestroyTexture(brickTexture);
        brickTexture = nullptr;
    }
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}