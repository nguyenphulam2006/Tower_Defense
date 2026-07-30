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
    ColorSwatch water {40, 100, 200, 255};
    ColorSwatch lava {213, 78, 13, 255};
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
        else if (key == "water") palette.water = color;
        else if (key == "lava") palette.lava = color;
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
    if (!TTF_Init()) {
        std::cout << "TTF_Init loi: " << SDL_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow("Tower Defense - SDL3 Modular", MAP_WIDTH * TILE_SIZE, MAP_HEIGHT * TILE_SIZE, 0);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) return false;
    font = TTF_OpenFont("assets/WebLetter.ttf", 24);
    if (!font) {
        font = TTF_OpenFont("../assets/WebLetter.ttf", 24);
    }
    if (!font) {
        std::cout << "Khong load duoc font: " << SDL_GetError() << std::endl;
    }
    loadPaletteFromFile(palette);
    grassTexture = loadTexture(
        renderer,
        "assets/spr_grass_02.png",
        "../assets/spr_grass_02.png"
    );

    brickTexture = loadTexture(
        renderer,
        "assets/brick.png",
        "../assets/brick.png"
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
    waterTexture = loadTexture(
        renderer,
        "assets/water.png",
        "../assets/water.png"
    );
    lavaTexture = loadTexture(
        renderer,
        "assets/lava.png",
        "../assets/lava.png"
    );
    enemyAnimatedTexture = loadTexture(
        renderer,
        "assets/spr_normal_slime.png",
        "../assets/spr_normal_slime.png"
    );
    towerArcherTexture = loadTexture(
        renderer,
        "assets/spr_tower_archer.png",
        "../assets/spr_tower_archer.png"
    );
    
    arrowProjectileTexture = loadTexture(
        renderer,
        "assets/spr_tower_archer_projectile.png",
        "../assets/spr_tower_archer_projectile.png"
    );
    bgMainMenuTexture = loadTexture(
        renderer,
        "assets/bg_main_menu.png",
        "../assets/bg_main_menu.png"
    );
    panelTexture = loadTexture(
        renderer,
        "assets/panel.png",
        "../assets/panel.png"
    );
    buttonGreenTexture = loadTexture(
        renderer,
        "assets/button_green.png",
        "../assets/button_green.png"
    );
    return true;
}

void RenderSystem::draw(const GameData& data) {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_Color textColor = {255, 255, 255, 255};
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
            else if (tile == TileType::Water) {
                if (waterTexture != nullptr) {
                    SDL_RenderTexture(renderer, waterTexture, nullptr, &cell);
                } else {
                    setColor(renderer, palette.water);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
            else if (tile == TileType::Lava) {
                if (lavaTexture != nullptr) {
                    SDL_RenderTexture(renderer, lavaTexture, nullptr, &cell);
                } else {
                    setColor(renderer, palette.lava);
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
 if (enemyAnimatedTexture != nullptr) {
            float texW, texH;
            SDL_GetTextureSize(enemyAnimatedTexture, &texW, &texH);
            int totalFrames = 4;
            float frameW = texW / totalFrames;
            Uint32 currentTime = SDL_GetTicks();
            int currentFrame = (currentTime / 100) % totalFrames;
            SDL_FRect srcRect = { currentFrame * frameW, 0.0f, frameW, texH };    
            SDL_FRect enemyDst = { 
                (float)enemyPos.x * TILE_SIZE + 16, 
                (float)enemyPos.y * TILE_SIZE + 16, 
                32.0f, 32.0f 
            };
            
            // Vẽ animation
            SDL_RenderTexture(renderer, enemyAnimatedTexture, &srcRect, &enemyDst);
        } else {
            setColor(renderer, palette.enemy);
            SDL_FRect enemyRect = { 
                (float)enemyPos.x * TILE_SIZE + 16, 
                (float)enemyPos.y * TILE_SIZE + 16, 
                32.0f, 32.0f 
            };
            SDL_RenderFillRect(renderer, &enemyRect);
        }
        float hpPercent = std::max(0.0f, (float)enemy.hp / enemy.maxHp);
        SDL_FRect bgBar = { (float)enemyPos.x * TILE_SIZE + 12, (float)enemyPos.y * TILE_SIZE + 48, 40.0f, 6.0f };
        SDL_FRect fgBar = { (float)enemyPos.x * TILE_SIZE + 12, (float)enemyPos.y * TILE_SIZE + 48, 40.0f * hpPercent, 6.0f };
        
        // Nền thanh máu (màu đỏ)
        SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        SDL_RenderFillRect(renderer, &bgBar);
        // Phần máu còn lại (màu xanh lá)
        SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
        SDL_RenderFillRect(renderer, &fgBar);
        // Viền thanh máu (màu đen)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderRect(renderer, &bgBar);
    }

    // Vẽ Operators
    for (const auto& op : data.operators) {
       SDL_FRect destRect = { 
            (float)op.pos.x * TILE_SIZE, 
            (float)op.pos.y * TILE_SIZE, 
            (float)TILE_SIZE, (float)TILE_SIZE 
        };
        if (op.type == TowerType::Basic && towerArcherTexture != nullptr) {
            SDL_RenderTexture(renderer, towerArcherTexture, nullptr, &destRect);
        } else {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 220);
            SDL_FRect opBorder = { destRect.x + 4, destRect.y + 4, 56.0f, 56.0f };
            SDL_RenderRect(renderer, &opBorder);

            if (op.type == TowerType::Basic) setColor(renderer, palette.towerBasic);
            else if (op.type == TowerType::Sniper) setColor(renderer, palette.towerSniper);
            else setColor(renderer, palette.towerSplash);
            
            SDL_FRect opRect = { destRect.x + 8, destRect.y + 8, 48.0f, 48.0f };
            SDL_RenderFillRect(renderer, &opRect);
        }
    }

    // Vẽ đạn
    for (const auto& projectile : data.projectiles) {
 if (projectile.sourceType == TowerType::Basic && arrowProjectileTexture != nullptr) {
            SDL_FRect bulletDst = { projectile.x - 8.0f, projectile.y - 8.0f, 16.0f, 16.0f };
            SDL_RenderTexture(renderer, arrowProjectileTexture, nullptr, &bulletDst);
        } else {
            if (projectile.sourceType == TowerType::Basic) setColor(renderer, palette.projectileBasic);
            else if (projectile.sourceType == TowerType::Sniper) setColor(renderer, palette.projectileSniper);
            else setColor(renderer, palette.projectileSplash);
            
            SDL_FRect bullet = { projectile.x - 4.0f, projectile.y - 4.0f, 8.0f, 8.0f };
            SDL_RenderFillRect(renderer, &bullet);
        }
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
                SDL_RenderTexture(renderer, heartAnimatedTexture, &srcRect, &heartDst);
            } else {
                setColor(renderer, {100, 100, 100, 255});
                SDL_RenderFillRect(renderer, &heartDst);
            }
        }
    }

 // ... (Các phần vẽ map, quái, trụ, đạn ở trên giữ nguyên) ...

    if (data.gameState != GameState::Playing) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        
        float mouseX = 0.0f, mouseY = 0.0f;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (data.gameState == GameState::MainMenu) {
            // 1. Vẽ hình nền Main Menu bao phủ toàn màn hình
            if (bgMainMenuTexture) {
                SDL_FRect bgRect = {0, 0, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE)};
                SDL_RenderTexture(renderer, bgMainMenuTexture, nullptr, &bgRect);
            } else {
                setColor(renderer, {100, 180, 255, 255}); // Bầu trời xanh
                SDL_FRect bgRect = {0, 0, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE)};
                SDL_RenderFillRect(renderer, &bgRect); 
            }

            // 2. Vẽ Tiêu đề Game
            renderText("TOWER DEFENSE", 150.0f, 150.0f, {255, 255, 255, 255});
            renderText("EPIC BATTLE", 180.0f, 200.0f, {200, 230, 255, 255});

            // Định nghĩa kích thước và vị trí của Panel (Bảng menu)
            float panelW = 320.0f;
            float panelH = 380.0f;
            float panelX = (MAP_WIDTH * TILE_SIZE) - panelW - 100.0f;
            float panelY = 200.0f;

            // 3. Vẽ Panel 
            SDL_FRect panelRect = {panelX, panelY, panelW, panelH};
            if (panelTexture) {
                SDL_RenderTexture(renderer, panelTexture, nullptr, &panelRect);
            } else {
                setColor(renderer, {140, 145, 150, 255}); // Màu xám đá
                SDL_RenderFillRect(renderer, &panelRect);
                setColor(renderer, {80, 85, 90, 255}); // Viền tối màu
                SDL_RenderRect(renderer, &panelRect);
            }

            renderText("MAIN MENU", panelX + 90.0f, panelY + 15.0f, {255, 255, 255, 255});

            // Thông số các nút bấm
            int btnW = 200, btnH = 60, gap = 30;
            float startBtnY = panelY + 80.0f;

            // Hàm lambda nội bộ để vẽ nút
            auto drawStyledButton = [&](int index, const std::string& text) {
                float btnX = panelX + (panelW - btnW) / 2.0f;
                float btnY = startBtnY + index * (btnH + gap);
                bool hover = mouseX >= btnX && mouseX <= btnX + btnW && mouseY >= btnY && mouseY <= btnY + btnH;

                SDL_FRect btnRect = {btnX, btnY, (float)btnW, (float)btnH};

                if (buttonGreenTexture) {
                    if (hover) SDL_SetTextureColorMod(buttonGreenTexture, 200, 255, 200);
                    else SDL_SetTextureColorMod(buttonGreenTexture, 255, 255, 255);
                    SDL_RenderTexture(renderer, buttonGreenTexture, nullptr, &btnRect);
                } else {
                    setColor(renderer, hover ? ColorSwatch{150, 220, 50, 255} : ColorSwatch{120, 200, 40, 255});
                    SDL_RenderFillRect(renderer, &btnRect);
                    setColor(renderer, {20, 50, 10, 255});
                    SDL_RenderRect(renderer, &btnRect);
                }

                renderText(text, btnX + btnW / 2.0f - text.length() * 6.0f + 2.0f, btnY + 18.0f, {0, 0, 0, 255});
                renderText(text, btnX + btnW / 2.0f - text.length() * 6.0f, btnY + 16.0f, {255, 255, 255, 255});
            };

            // 4. Vẽ 3 nút bấm
            drawStyledButton(0, "PLAY");
            drawStyledButton(1, "OPTIONS");
            drawStyledButton(2, "QUIT");

        }
        else if (data.gameState == GameState::Settings) {
            // Tạm thời vẽ màn mờ cho Cài đặt
            setColor(renderer, palette.overlay);
            SDL_FRect overlay = { 0.0f, 0.0f, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE) };
            SDL_RenderFillRect(renderer, &overlay);
            renderText("SETTINGS", 400.0f, 200.0f, {255, 255, 255, 255});
        }
        else if (data.gameState == GameState::Paused || data.gameState == GameState::GameOver) {
            setColor(renderer, palette.overlay);
            SDL_FRect overlay = { 0.0f, 0.0f, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE) };
            SDL_RenderFillRect(renderer, &overlay);
            
            int btnX = (MAP_WIDTH * TILE_SIZE - MENU_BUTTON_WIDTH) / 2;
            int btnY = (MAP_HEIGHT * TILE_SIZE - MENU_BUTTON_HEIGHT) / 2;
            
            bool hover = mouseX >= btnX && mouseX < btnX + MENU_BUTTON_WIDTH && mouseY >= btnY && mouseY < btnY + MENU_BUTTON_HEIGHT;

            setColor(renderer, hover ? palette.buttonRetryHover : palette.buttonRetry);
            SDL_FRect menuButton = { (float)btnX, (float)btnY, (float)MENU_BUTTON_WIDTH, (float)MENU_BUTTON_HEIGHT };
            SDL_RenderFillRect(renderer, &menuButton);

            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            SDL_RenderRect(renderer, &menuButton);

            std::string stateTitle = "Tower Defense | ";
            if (data.gameState == GameState::Paused) stateTitle += "PAUSED";
            else if (data.gameState == GameState::GameOver) stateTitle += "GAME OVER";
            
            renderText(stateTitle, (float)btnX - 40.0f, (float)btnY - 60.0f, {255, 255, 255, 255});
            renderText("RETRY / START", (float)btnX + 30.0f, (float)btnY + 12.0f, {255, 255, 255, 255});
        }
    } 
    else {
        // KHỐI LỆNH NÀY SẼ HIỂN THỊ CHỈ SỐ KHI ĐANG CHƠI GAME
        std::string waveText = "Wave: " + std::to_string(data.currentWave);
        std::string hpText = "HP: " + std::to_string(data.baseHP);
        std::string goldText = "Gold: " + std::to_string(data.gold);
        
        // Tạo thêm text hiển thị giới hạn tháp
        std::string towersInfo = "Towers: " + std::to_string(data.operators.size()) + "/" + std::to_string(data.maxTowers);
        
        std::string towerText = "Selected Tower: ";
        if (data.selectedTowerType == TowerType::Basic) towerText += "Basic ($25)";
        else if (data.selectedTowerType == TowerType::Sniper) towerText += "Sniper ($40)";
        else if (data.selectedTowerType == TowerType::Splash) towerText += "Splash ($35)";

        SDL_Color textColor = {255, 255, 255, 255};
        renderText(waveText, 20.0f, MAP_HEIGHT * TILE_SIZE - 40.0f, {200, 255, 200, 255});
        renderText(goldText, 120.0f, MAP_HEIGHT * TILE_SIZE - 40.0f, {255, 220, 50, 255});
        renderText(towersInfo, 250.0f, MAP_HEIGHT * TILE_SIZE - 40.0f, {100, 200, 255, 255}); 
        renderText(towerText, 450.0f, MAP_HEIGHT * TILE_SIZE - 40.0f, textColor); 
    }
    SDL_RenderPresent(renderer);
}
void RenderSystem::renderText(const std::string& text, float x, float y, SDL_Color color, bool isStatic) {
    if (!font) return; 

    SDL_Texture* textTexture = nullptr;

    if (isStatic) {
        std::string cacheKey = text + "_" + std::to_string(color.r) + std::to_string(color.g) + std::to_string(color.b);
        if (textCache.find(cacheKey) == textCache.end()) {
            SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), text.length(), color);
            if (textSurface) {
                textCache[cacheKey] = SDL_CreateTextureFromSurface(renderer, textSurface);
                SDL_DestroySurface(textSurface);
            }
        }
        textTexture = textCache[cacheKey];
    } else {
        SDL_Surface* textSurface = TTF_RenderText_Blended(font, text.c_str(), text.length(), color);
        if (textSurface) {
            textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_DestroySurface(textSurface);
        }
    }
    if (textTexture) {
        float texW, texH;
        SDL_GetTextureSize(textTexture, &texW, &texH);
        SDL_FRect renderQuad = { x, y, texW, texH };
        SDL_RenderTexture(renderer, textTexture, nullptr, &renderQuad);
        if (!isStatic) {
            SDL_DestroyTexture(textTexture);
        }
    }
}
void RenderSystem::cleanup() {
    for (auto& pair : textCache) {
        SDL_DestroyTexture(pair.second);
    }
    textCache.clear();

    if (heartAnimatedTexture != nullptr) {
        SDL_DestroyTexture(heartAnimatedTexture);
        heartAnimatedTexture = nullptr;
    }
    if (heartTexture != nullptr) {
        SDL_DestroyTexture(heartTexture);
        heartTexture = nullptr;
    }
    if (waterTexture != nullptr) {
        SDL_DestroyTexture(waterTexture);
        waterTexture = nullptr;
    }
    if (lavaTexture != nullptr) {
        SDL_DestroyTexture(lavaTexture);
        lavaTexture = nullptr;
    }
    if (grassTexture != nullptr) {
        SDL_DestroyTexture(grassTexture);
        grassTexture = nullptr;
    }
    if (brickTexture != nullptr) {
        SDL_DestroyTexture(brickTexture);
        brickTexture = nullptr;
    }
    if (enemyAnimatedTexture != nullptr) {
        SDL_DestroyTexture(enemyAnimatedTexture);
        enemyAnimatedTexture = nullptr;
    }
    if (towerArcherTexture != nullptr) {
        SDL_DestroyTexture(towerArcherTexture);
        towerArcherTexture = nullptr;
    }
    if (arrowProjectileTexture != nullptr) {
        SDL_DestroyTexture(arrowProjectileTexture);
        arrowProjectileTexture = nullptr;
    }
    if (bgMainMenuTexture) SDL_DestroyTexture(bgMainMenuTexture);
    if (panelTexture) SDL_DestroyTexture(panelTexture);
    if (buttonGreenTexture) SDL_DestroyTexture(buttonGreenTexture);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}