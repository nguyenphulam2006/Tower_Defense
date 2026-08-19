#include "RenderSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>

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
    ColorSwatch towerFrost {100, 200, 255, 255};    
    ColorSwatch towerBlocker {150, 100, 50, 255};  
    ColorSwatch projectileBasic {255, 220, 0, 255};
    ColorSwatch projectileFrost {200, 255, 255, 255}; 
    ColorSwatch projectileBlocker {150, 100, 50, 255}; 
    ColorSwatch buttonRetry {200, 60, 60, 255};
    ColorSwatch buttonRetryHover {230, 90, 90, 255};
    ColorSwatch overlay {0, 0, 0, 160};
    ColorSwatch water {40, 100, 200, 255};
    ColorSwatch lava {213, 78, 13, 255};
};

TilePalette palette;

void setColor(SDL_Renderer* renderer, const ColorSwatch& color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
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
        int r = 0, g = 0, b = 0, a = 255;
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
        else if (key == "tower_frost") palette.towerFrost = color;
        else if (key == "tower_blocker") palette.towerBlocker = color;
        else if (key == "projectile_basic") palette.projectileBasic = color;
        else if (key == "projectile_frost") palette.projectileFrost = color;
        else if (key == "projectile_blocker") palette.projectileBlocker = color;
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
    
    assets.loadFont("title_font", "assets/font_title.ttf", 48, "../assets/font_title.ttf");
    assets.loadFont("main_font", "assets/font_ui.ttf", 18, "../assets/font_ui.ttf");
    if (!assets.getFont("main_font") || !assets.getFont("title_font")) {
        std::cout << "Luu y: Co loi load font hoac thieu file font: " << SDL_GetError() << std::endl;
    }

    loadPaletteFromFile(palette);
    
    assets.loadTexture(renderer, "grass", "assets/spr_grass_02.png", "../assets/spr_grass_02.png");
    assets.loadTexture(renderer, "brick", "assets/brick.png", "../assets/brick.png");
    assets.loadTexture(renderer, "heart", "assets/heart.png", "../assets/heart.png");
    assets.loadTexture(renderer, "heart_animated", "assets/heart_animated_1.png", "../assets/heart_animated_1.png");
    assets.loadTexture(renderer, "water", "assets/water.png", "../assets/water.png");
    assets.loadTexture(renderer, "lava", "assets/lava.png", "../assets/lava.png");
    assets.loadTexture(renderer, "enemy_slime", "assets/spr_normal_slime.png", "../assets/spr_normal_slime.png");
    assets.loadTexture(renderer, "tower_archer", "assets/spr_tower_archer.png", "../assets/spr_tower_archer.png");
    assets.loadTexture(renderer, "tower_archer_alt", "assets/spr_tower_crossbow.png", "../assets/spr_tower_crossbow.png");
    assets.loadTexture(renderer, "arrow_projectile", "assets/spr_tower_archer_projectile.png", "../assets/spr_tower_archer_projectile.png");
    assets.loadTexture(renderer, "arrow_projectile_alt", "assets/spr_tower_crossbow_projectile.png", "../assets/spr_tower_crossbow_projectile.png");
    assets.loadTexture(renderer, "frost_projectile", "assets/spr_tower_ice_wizard_projectile.png", "../assets/spr_tower_ice_wizard_projectile.png");
    assets.loadTexture(renderer, "frost_projectile_alt", "assets/spr_tower_poison_wizard_projectile.png", "../assets/spr_tower_poison_wizard_projectile.png");
    assets.loadTexture(renderer, "tower_frost", "assets/spr_tower_ice_wizard.png", "../assets/spr_tower_ice_wizard.png");
    assets.loadTexture(renderer, "tower_frost_alt", "assets/spr_tower_poison_wizard.png", "../assets/spr_tower_poison_wizard.png");
    assets.loadTexture(renderer, "tower_cannon", "assets/spr_tower_cannon.png", "../assets/spr_tower_cannon.png");
    assets.loadTexture(renderer, "cannon_projectile", "assets/spr_tower_cannon_projectile.png", "../assets/spr_tower_cannon_projectile.png");
    assets.loadTexture(renderer, "tower_electric", "assets/spr_tower_lightning_tower.png", "../assets/spr_tower_lightning_tower.png");
    assets.loadTexture(renderer, "tower_electric_alt", "assets/spr_tower_poison_wizard.png", "../assets/spr_tower_poison_wizard.png");
    assets.loadTexture(renderer, "electric_projectile", "assets/spr_tower_lightning_tower_projectile.png", "../assets/spr_tower_lightning_tower_projectile.png");
    assets.loadTexture(renderer, "tower_tesla", "assets/spr_tower_poison_wizard.png", "../assets/spr_tower_poison_wizard.png");
    assets.loadTexture(renderer, "tesla_projectile", "assets/spr_tower_poison_wizard_projectile.png", "../assets/spr_tower_poison_wizard_projectile.png");
    assets.loadTexture(renderer, "bg_main_menu", "assets/bg_main_menu.png", "../assets/bg_main_menu.png");
    assets.loadTexture(renderer, "enemy_goblin", "assets/spr_goblin.png", "../assets/spr_goblin.png");
    assets.loadTexture(renderer, "enemy_demon", "assets/spr_demon.png", "../assets/spr_demon.png");
    assets.loadTexture(renderer, "enemy_ghost", "assets/spr_ghost.png", "../assets/spr_ghost.png");
    assets.loadTexture(renderer, "enemy_zombie", "assets/spr_zombie.png", "../assets/spr_zombie.png");
    assets.loadTexture(renderer, "enemy_skeleton", "assets/spr_skeleton.png", "../assets/spr_skeleton.png");
    assets.loadTexture(renderer, "enemy_bat", "assets/spr_bat.png", "../assets/spr_bat.png");
    assets.loadTexture(renderer, "enemy_king_slime", "assets/spr_king_slime.png", "../assets/spr_king_slime.png");
    assets.loadTexture(renderer, "enemy_big_slime", "assets/spr_big_slime.png", "../assets/spr_big_slime.png");
    assets.loadTexture(renderer, "enemy_normal_slime", "assets/spr_normal_slime.png", "../assets/spr_normal_slime.png");
    return true;
}

void RenderSystem::draw(const GameData& data) {
    SDL_SetRenderDrawColor(renderer, 40, 40, 40, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Ve Map
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            TileType tile = data.tileMap[y][x];
            SDL_FRect cell = { (float)x * TILE_SIZE, (float)y * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
            if (tile == TileType::Grass) {
                if (SDL_Texture* tex = assets.getTexture("grass")) {
                    SDL_RenderTexture(renderer, tex, nullptr, &cell);
                } else {
                    setColor(renderer, {palette.grass.r, palette.grass.g, palette.grass.b, 255});
                    SDL_RenderFillRect(renderer, &cell);
                }
            } else if (tile == TileType::Path || tile == TileType::Base) {
                if (SDL_Texture* tex = assets.getTexture("brick")) {
                    SDL_RenderTexture(renderer, tex, nullptr, &cell);
                } else {
                    setColor(renderer, {palette.path.r, palette.path.g, palette.path.b, 255});
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
            else if (tile == TileType::Water) {
                if (SDL_Texture* tex = assets.getTexture("water")) {
                    SDL_RenderTexture(renderer, tex, nullptr, &cell);
                } else {
                    setColor(renderer, palette.water);
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
            else if (tile == TileType::Lava) {
                if (SDL_Texture* tex = assets.getTexture("lava")) {
                    SDL_RenderTexture(renderer, tex, nullptr, &cell);
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

 // Ve Quai vat
    for (const auto& enemy : data.enemies) {
        if (!enemy.active) continue;
        Position enemyPos = data.enemyPath[enemy.currentStep];
        SDL_FRect enemyDst = { (float)enemyPos.x * TILE_SIZE + 16, (float)enemyPos.y * TILE_SIZE + 16, 32.0f, 32.0f };

        std::string textureId;
        switch (enemy.type) {
            case EnemyType::Goblin:   textureId = "enemy_goblin"; break;
            case EnemyType::Demon:    textureId = "enemy_demon"; break;
            case EnemyType::Ghost:    textureId = "enemy_ghost"; break;
            case EnemyType::Zombie:   
            case EnemyType::Tanker:   textureId = "enemy_zombie"; break;
            case EnemyType::Skeleton: textureId = "enemy_skeleton"; break;
            case EnemyType::Bat:      
            case EnemyType::Fast:     textureId = "enemy_bat"; break;
            case EnemyType::BigSlime: textureId = "enemy_big_slime"; break;
            case EnemyType::KingSlime:
            case EnemyType::Boss:     textureId = "enemy_king_slime"; break;
            case EnemyType::Normal:
            default:                  textureId = "enemy_normal_slime"; break;
        }

        if (SDL_Texture* tex = assets.getTexture(textureId)) {
            float texW, texH;
            SDL_GetTextureSize(tex, &texW, &texH);
            
            int totalFrames = 4;  
            float frameW = texW / totalFrames;
            Uint32 currentTime = SDL_GetTicks();
            int currentFrame = (currentTime / 100) % totalFrames;  
            SDL_FRect srcRect = { currentFrame * frameW, 0.0f, frameW, texH };    
            
            if (enemy.slowTimer > 0) {
                SDL_SetTextureColorMod(tex, 100, 150, 255); 
            } else {
                SDL_SetTextureColorMod(tex, 255, 255, 255); 
            }
            
            SDL_RenderTexture(renderer, tex, &srcRect, &enemyDst);
        } else {
            setColor(renderer, {255, 0, 0, 255});
            SDL_RenderFillRect(renderer, &enemyDst);
        }

        float hpPercent = std::max(0.0f, (float)enemy.hp / enemy.maxHp);
        float hpBarY = (float)enemyPos.y * TILE_SIZE - 10.0f;
        SDL_FRect bgBar = { (float)enemyPos.x * TILE_SIZE + 12, hpBarY, 40.0f, 6.0f };
        SDL_FRect fgBar = { (float)enemyPos.x * TILE_SIZE + 12, hpBarY, 40.0f * hpPercent, 6.0f };
        SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
        SDL_RenderFillRect(renderer, &bgBar);
        SDL_SetRenderDrawColor(renderer, 50, 200, 50, 255);
        SDL_RenderFillRect(renderer, &fgBar);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderRect(renderer, &bgBar);
    }

    // Ve Operators
    for (const auto& op : data.operators) {
        SDL_FRect destRect = { (float)op.pos.x * TILE_SIZE, (float)op.pos.y * TILE_SIZE, (float)TILE_SIZE, (float)TILE_SIZE };
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 220);
        SDL_FRect opBorder = { destRect.x + 4, destRect.y + 4, 56.0f, 56.0f };
        SDL_RenderRect(renderer, &opBorder);
        SDL_FRect opRect = { destRect.x + 8, destRect.y + 8, 48.0f, 48.0f };

        SDL_Texture* texToDraw = nullptr;
        if (op.type == TowerType::Basic) texToDraw = assets.getTexture("tower_archer");
        else if (op.type == TowerType::Frost) texToDraw = assets.getTexture("tower_frost");
        else if (op.type == TowerType::Electric) texToDraw = assets.getTexture("tower_electric");
        else if (op.type == TowerType::Cannon) texToDraw = assets.getTexture("tower_cannon");
        else if (op.type == TowerType::Tesla) texToDraw = assets.getTexture("tower_tesla");

        if (texToDraw != nullptr) {
            SDL_RenderTexture(renderer, texToDraw, nullptr, &opRect);
        } else {
            if (op.type == TowerType::Basic) setColor(renderer, palette.towerBasic);
            else if (op.type == TowerType::Frost) setColor(renderer, palette.towerFrost);
            else if (op.type == TowerType::Electric) SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255);
            else if (op.type == TowerType::Cannon) SDL_SetRenderDrawColor(renderer, 255, 120, 0, 255);
            else if (op.type == TowerType::Tesla) SDL_SetRenderDrawColor(renderer, 100, 200, 255, 255);
            else SDL_SetRenderDrawColor(renderer, 50, 150, 50, 255);  
            SDL_RenderFillRect(renderer, &opRect);
        }
        
        if (op.level > 1) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            SDL_FRect levelBg = { destRect.x + 40, destRect.y + 4, 16.0f, 16.0f };
            SDL_RenderFillRect(renderer, &levelBg);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderRect(renderer, &levelBg);
        }
    }

    // Ve Dan
    for (const auto& projectile : data.projectiles) {
        SDL_Texture* texArrow = assets.getTexture("arrow_projectile");
        SDL_Texture* texFrost = assets.getTexture("frost_projectile");
        SDL_Texture* texCannon = assets.getTexture("cannon_projectile");
        SDL_Texture* texElectric = assets.getTexture("electric_projectile");
        SDL_Texture* texTesla = assets.getTexture("tesla_projectile");

        SDL_Texture* texProjectile = nullptr;
        if (projectile.sourceType == TowerType::Basic) texProjectile = texArrow;
        else if (projectile.sourceType == TowerType::Frost) texProjectile = texFrost;
        else if (projectile.sourceType == TowerType::Cannon) texProjectile = texCannon;
        else if (projectile.sourceType == TowerType::Electric) texProjectile = texElectric;
        else if (projectile.sourceType == TowerType::Tesla) texProjectile = texTesla;

        SDL_FRect bulletDst = { projectile.x - 10.0f, projectile.y - 10.0f, 20.0f, 20.0f };
        if (texProjectile != nullptr) {
            SDL_RenderTexture(renderer, texProjectile, nullptr, &bulletDst);
        }

        if (texProjectile == nullptr) {
            if (projectile.sourceType == TowerType::Basic) setColor(renderer, palette.projectileBasic);
            else if (projectile.sourceType == TowerType::Frost) setColor(renderer, palette.projectileFrost);
            else if (projectile.sourceType == TowerType::Electric) SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);
            else if (projectile.sourceType == TowerType::Cannon) SDL_SetRenderDrawColor(renderer, 255, 150, 0, 255);
            else if (projectile.sourceType == TowerType::Tesla) SDL_SetRenderDrawColor(renderer, 120, 255, 120, 255);
            else setColor(renderer, palette.projectileBlocker);
            SDL_RenderFillRect(renderer, &bulletDst);
        }
    }

    // Vẽ Particles (hiệu ứng)
    for (const auto& particle : data.particles) {
        if (!particle.active) continue;
        float alpha = 1.0f - (particle.lifetime / particle.maxLifetime);
        SDL_SetRenderDrawColor(renderer, particle.r, particle.g, particle.b, (Uint8)(255 * alpha));
        SDL_FRect pRect = { particle.x - particle.scale * 2.0f, particle.y - particle.scale * 2.0f, 
                           particle.scale * 4.0f, particle.scale * 4.0f };
        SDL_RenderFillRect(renderer, &pRect);
    }

    // UI Mang
    int maxHearts = 3;
    float heartSize = 40.0f, heartGap = 10.0f, startX = 16.0f, startY = 16.0f;
    for (int i = 0; i < maxHearts; ++i) {
        SDL_FRect heartDst = { startX + i * (heartSize + heartGap), startY, heartSize, heartSize };
        
        if (i < data.baseHP) {
            if (SDL_Texture* tex = assets.getTexture("heart")) {
                SDL_RenderTexture(renderer, tex, nullptr, &heartDst);
            } else {
                setColor(renderer, {255, 50, 50, 255});
                SDL_RenderFillRect(renderer, &heartDst);
            }
        } else {
            if (SDL_Texture* texAnim = assets.getTexture("heart_animated")) {
                int totalFrames = 5; 
                float texW, texH;
                SDL_GetTextureSize(texAnim, &texW, &texH);
                float frameW = texW / totalFrames;
                
                Uint64 timeSinceLost = SDL_GetTicks() - data.lostHeartTime[i];
                int currentFrame = (int)(timeSinceLost / 150);
                if (currentFrame >= totalFrames) currentFrame = totalFrames - 1; 
                
                SDL_FRect srcRect = { currentFrame * frameW, 0.0f, frameW, texH };
                SDL_RenderTexture(renderer, texAnim, &srcRect, &heartDst);
            } else {
                setColor(renderer, {100, 100, 100, 255});
                SDL_RenderFillRect(renderer, &heartDst);
            }
        }
    }

    if (data.gameState != GameState::Playing) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        float mouseX = 0.0f, mouseY = 0.0f;
        SDL_GetMouseState(&mouseX, &mouseY);

        if (data.gameState == GameState::MainMenu || data.gameState == GameState::Settings) {
            if (SDL_Texture* texBg = assets.getTexture("bg_main_menu")) {
                SDL_FRect bgRect = {0, 0, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE)};
                SDL_RenderTexture(renderer, texBg, nullptr, &bgRect);
            } else {
                setColor(renderer, {100, 180, 255, 255}); 
                SDL_FRect bgRect = {0, 0, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE)};
                SDL_RenderFillRect(renderer, &bgRect); 
            }
        }

        float panelW = 320.0f, panelH = 380.0f;
        float panelX = (MAP_WIDTH * TILE_SIZE - panelW) / 2.0f;
        float panelY = (MAP_HEIGHT * TILE_SIZE - panelH) / 2.0f;
        float centerX = (MAP_WIDTH * TILE_SIZE) / 2.0f; 
        
        float btnW = 200.0f, btnH = 60.0f, gap = 30.0f;
        float startBtnY = panelY + 80.0f;

        auto drawCodePanel = [&]() {
            SDL_FRect panelShadow = {panelX + 8.0f, panelY + 8.0f, panelW, panelH};
            setColor(renderer, {0, 0, 0, 60});
            SDL_RenderFillRect(renderer, &panelShadow);

            SDL_FRect panelRect = {panelX, panelY, panelW, panelH};
            setColor(renderer, {245, 245, 245, 230}); 
            SDL_RenderFillRect(renderer, &panelRect);

            setColor(renderer, {180, 180, 180, 255});
            SDL_RenderRect(renderer, &panelRect);

            SDL_FRect innerBorder = {panelX + 3.0f, panelY + 3.0f, panelW - 6.0f, panelH - 6.0f};
            setColor(renderer, {255, 255, 255, 255});
            SDL_RenderRect(renderer, &innerBorder);
        };

        auto drawCodeButton = [&](int index, const std::string& text, float customStartY) {
            float btnX = panelX + (panelW - btnW) / 2.0f;
            float btnY = customStartY + index * (btnH + gap);
            bool hover = mouseX >= btnX && mouseX <= btnX + btnW && mouseY >= btnY && mouseY <= btnY + btnH;

            SDL_FRect shadowRect = {btnX + 4.0f, btnY + 4.0f, btnW, btnH};
            setColor(renderer, {0, 0, 0, 50});
            SDL_RenderFillRect(renderer, &shadowRect);

            SDL_FRect btnRect = {btnX, btnY, btnW, btnH};
            setColor(renderer, hover ? ColorSwatch{110, 210, 60, 255} : ColorSwatch{90, 190, 50, 255});
            SDL_RenderFillRect(renderer, &btnRect);

            SDL_FRect bottomEdge = {btnX, btnY + btnH - 6.0f, btnW, 6.0f};
            setColor(renderer, hover ? ColorSwatch{80, 170, 40, 255} : ColorSwatch{60, 140, 30, 255});
            SDL_RenderFillRect(renderer, &bottomEdge);

            setColor(renderer, {30, 80, 15, 255}); 
            SDL_RenderRect(renderer, &btnRect);

            float textCenterY = btnY + 20.0f; 
            // Căn giữa chữ hoàn hảo 100% nhờ hàm isCentered = true ở cuối
            renderText(text, centerX, textCenterY + 2.0f, {30, 80, 15, 255}, false, "main_font", true); 
            renderText(text, centerX, textCenterY, {255, 255, 255, 255}, false, "main_font", true);             
        };

        if (data.gameState == GameState::MainMenu) {
            // Đưa chữ vào đúng tâm màn hình (centerX) và bật cờ căn giữa
            renderText("TOWER DEFENSE", centerX, panelY - 80.0f, {20, 20, 20, 255}, false, "title_font", true);
            renderText("TOWER DEFENSE", centerX, panelY - 82.0f, {255, 255, 255, 255}, false, "title_font", true);
            renderText("EPIC BATTLE", centerX, panelY - 30.0f, {200, 230, 255, 255}, false, "main_font", true);

            drawCodePanel();

            renderText("MAIN MENU", centerX, panelY + 15.0f, {50, 50, 50, 255}, false, "main_font", true);

            drawCodeButton(0, "PLAY", startBtnY);
            drawCodeButton(1, "OPTIONS", startBtnY);
            drawCodeButton(2, "QUIT", startBtnY);
        }
        else if (data.gameState == GameState::Settings) {
            drawCodePanel();

            renderText("OPTIONS", centerX, panelY + 15.0f, {50, 50, 50, 255}, false, "main_font", true);

            std::string soundText = data.soundEnabled ? "SOUND: ON" : "SOUND: OFF";
            drawCodeButton(0, soundText, startBtnY);
            drawCodeButton(1, "BACK", startBtnY);
        }
 else if (data.gameState == GameState::Paused || data.gameState == GameState::GameOver) {
            // Hiệu ứng làm mờ nền
            setColor(renderer, palette.overlay);
            SDL_FRect overlay = { 0.0f, 0.0f, (float)(MAP_WIDTH * TILE_SIZE), (float)(MAP_HEIGHT * TILE_SIZE) };
            SDL_RenderFillRect(renderer, &overlay);
            
            float centerX = (MAP_WIDTH * TILE_SIZE) / 2.0f;
            float centerY = (MAP_HEIGHT * TILE_SIZE) / 2.0f;
            
            // Kích thước và vị trí 2 nút (phải khớp 100% với file InputSystem)
            float btnW = 240.0f, btnH = 50.0f;
            float btnX = centerX - btnW / 2.0f;
            float btn1Y = centerY - 40.0f;
            float btn2Y = centerY + 30.0f;

            bool hover1 = mouseX >= btnX && mouseX < btnX + btnW && mouseY >= btn1Y && mouseY < btn1Y + btnH;
            bool hover2 = mouseX >= btnX && mouseX < btnX + btnW && mouseY >= btn2Y && mouseY < btn2Y + btnH;

            // --- Vẽ nút 1 (RESUME / RETRY) ---
            setColor(renderer, hover1 ? palette.buttonRetryHover : palette.buttonRetry);
            SDL_FRect btn1Rect = { btnX, btn1Y, btnW, btnH };
            SDL_RenderFillRect(renderer, &btn1Rect);
            setColor(renderer, {255, 255, 255, 255}); // Viền trắng
            SDL_RenderRect(renderer, &btn1Rect);

            // --- Vẽ nút 2 (MAIN MENU) ---
            setColor(renderer, hover2 ? palette.buttonRetryHover : palette.buttonRetry);
            SDL_FRect btn2Rect = { btnX, btn2Y, btnW, btnH };
            SDL_RenderFillRect(renderer, &btn2Rect);
            setColor(renderer, {255, 255, 255, 255}); // Viền trắng
            SDL_RenderRect(renderer, &btn2Rect);

            // --- Đặt Tiêu Đề và Chữ cho nút ---
            std::string stateTitle = "";
            std::string btn1Text = "";
            
            if (data.gameState == GameState::Paused) {
                stateTitle = "PAUSED";
                btn1Text = "RESUME";
            } else {
                stateTitle = "GAME OVER";
                btn1Text = "RETRY";
            }
            
            // Vẽ Tiêu đề to
            renderText(stateTitle, centerX, btn1Y - 70.0f, {255, 255, 255, 255}, false, "title_font", true);
            
            // Vẽ chữ 2 nút
            renderText(btn1Text, centerX, btn1Y + 15.0f, {255, 255, 255, 255}, false, "main_font", true);
            renderText("MAIN MENU", centerX, btn2Y + 15.0f, {255, 255, 255, 255}, false, "main_font", true);
        }
    }
    else {
        // Game UI Bar
        SDL_FRect uiBg = { 0.0f, (float)(MAP_HEIGHT * TILE_SIZE - 50), (float)(MAP_WIDTH * TILE_SIZE), 50.0f };
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); 
        SDL_RenderFillRect(renderer, &uiBg);
        
        std::string waveText = "Wave: " + std::to_string(data.currentWave);
        std::string goldText = "Gold: " + std::to_string(data.gold);
        std::string enemyText = "Enemies: " + std::to_string(data.enemies.size());
        std::string towersInfo = "Towers: " + std::to_string(data.operators.size()) + "/" + std::to_string(data.maxTowers);
        
        std::string speedText = "Speed: ";
        if (data.gameSpeed == 0.5f) speedText += "0.5x";
        else if (data.gameSpeed == 1.0f) speedText += "1.0x";
        else if (data.gameSpeed == 1.5f) speedText += "1.5x";
        else if (data.gameSpeed == 2.0f) speedText += "2.0x";
        else speedText += std::to_string(data.gameSpeed) + "x";
        
        SDL_Color uiTextColor = {255, 255, 255, 255};
        
        renderText(waveText, 15.0f, MAP_HEIGHT * TILE_SIZE - 45.0f, {200, 255, 200, 255});
        renderText(goldText, 120.0f, MAP_HEIGHT * TILE_SIZE - 45.0f, {255, 220, 50, 255});
        renderText(enemyText, 230.0f, MAP_HEIGHT * TILE_SIZE - 45.0f, {255, 100, 100, 255}); 
        renderText(towersInfo, 350.0f, MAP_HEIGHT * TILE_SIZE - 45.0f, {100, 200, 255, 255});
        renderText(speedText, 540.0f, MAP_HEIGHT * TILE_SIZE - 45.0f, {200, 200, 100, 255});
        
        renderText("Right-Click=Sell | Mid-Click=Upgrade | ESC=Pause", 15.0f, MAP_HEIGHT * TILE_SIZE - 20.0f, {180, 180, 180, 255});

        float panelW = 150.0f, panelH = 280.0f;
        float panelX = (MAP_WIDTH * TILE_SIZE) - panelW - 10.0f, panelY = 10.0f;

        SDL_FRect towerPanel = {panelX, panelY, panelW, panelH};
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); 
        SDL_RenderFillRect(renderer, &towerPanel);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); 
        SDL_RenderRect(renderer, &towerPanel);

        // Áp dụng thuật toán căn giữa hoàn hảo cho chữ "ARMORY"
        float armoryCenterX = panelX + panelW / 2.0f;
        renderText("ARMORY", armoryCenterX, panelY + 10.0f, {255, 200, 0, 255}, false, "main_font", true);

        float btnX = panelX + 10.0f, btnW = 130.0f, btnH = 35.0f;

        SDL_FRect btnBasic = {btnX, panelY + 40.0f, btnW, btnH};
        setColor(renderer, data.selectedTowerType == TowerType::Basic ? ColorSwatch{100, 255, 100, 255} : ColorSwatch{50, 150, 50, 255});
        SDL_RenderFillRect(renderer, &btnBasic);
        renderText("1:Basic $25", btnX + 5.0f, panelY + 45.0f, uiTextColor);

        SDL_FRect btnFrost = {btnX, panelY + 75.0f, btnW, btnH};
        setColor(renderer, data.selectedTowerType == TowerType::Frost ? ColorSwatch{100, 200, 255, 255} : ColorSwatch{50, 100, 150, 255});
        SDL_RenderFillRect(renderer, &btnFrost);
        renderText("2:Frost $35", btnX + 5.0f, panelY + 80.0f, uiTextColor);

        SDL_FRect btnElectric = {btnX, panelY + 110.0f, btnW, btnH};
        setColor(renderer, data.selectedTowerType == TowerType::Electric ? ColorSwatch{150, 200, 255, 255} : ColorSwatch{80, 120, 180, 255});
        SDL_RenderFillRect(renderer, &btnElectric);
        renderText("3:Elec $45", btnX + 5.0f, panelY + 115.0f, uiTextColor);

        SDL_FRect btnCannon = {btnX, panelY + 145.0f, btnW, btnH};
        setColor(renderer, data.selectedTowerType == TowerType::Cannon ? ColorSwatch{255, 180, 50, 255} : ColorSwatch{200, 120, 20, 255});
        SDL_RenderFillRect(renderer, &btnCannon);
        renderText("4:Canon $55", btnX + 5.0f, panelY + 150.0f, uiTextColor);

        SDL_FRect btnTesla = {btnX, panelY + 180.0f, btnW, btnH};
        setColor(renderer, data.selectedTowerType == TowerType::Tesla ? ColorSwatch{120, 255, 120, 255} : ColorSwatch{70, 160, 80, 255});
        SDL_RenderFillRect(renderer, &btnTesla);
        renderText("5:Poison $50", btnX + 5.0f, panelY + 185.0f, uiTextColor);
    }
    SDL_RenderPresent(renderer);
}
void RenderSystem::renderText(const std::string& text, float x, float y, SDL_Color color, bool isStatic, const std::string& fontId, bool isCentered) {
    TTF_Font* font = assets.getFont(fontId);
    if (!font) return; 

    SDL_Texture* textTexture = nullptr;

    if (isStatic) {
        std::string cacheKey = text + "_" + std::to_string(color.r) + std::to_string(color.g) + std::to_string(color.b) + "_" + fontId;
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
        
        // Cập nhật tính năng lấy đúng chiều dài ảnh của đoạn chữ và dịch ngược về tâm
        float finalX = isCentered ? (x - texW / 2.0f) : x;
        
        SDL_FRect renderQuad = { finalX, y, texW, texH };
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

    assets.clear(); 

    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}