#include "InputSystem.h"
#include <iostream>
#include "MapSystem.h"
namespace {

bool isOnPath(const GameData& data, int gridX, int gridY) {
    for (const auto& p : data.enemyPath) {
        if (p.x == gridX && p.y == gridY) {
            return true;
        }
    }
    return false;
}
bool hasOperatorAt(const GameData& data, int gridX, int gridY) {
        for (const auto& op : data.operators) {
            if (op.pos.x == gridX && op.pos.y == gridY) return true;
        }
        return false;
    }
bool isInsideRect(int x, int y, int rectX, int rectY, int rectW, int rectH) {
        return x >= rectX && x < rectX + rectW && y >= rectY && y < rectY + rectH;
    }
bool isBuildableTile(const GameData& data, int gridX, int gridY, TowerType type) {
        if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) return false;
        TileType tile = data.tileMap[gridY][gridX];
        return tile == TileType::Grass;
    }
}
void InputSystem::handleInput(SDL_Event& event, GameData& data) {
    if (event.type == SDL_EVENT_MOUSE_MOTION && data.gameState == GameState::Playing) {
        int gridX = (int)(event.motion.x / TILE_SIZE);
        int gridY = (int)(event.motion.y / TILE_SIZE);
        data.hoveredGridX = gridX;
        data.hoveredGridY = gridY;
        data.hoveredGridBuildable = gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT && data.tileMap[gridY][gridX] == TileType::Grass;
    }

    if (event.type == SDL_EVENT_KEY_DOWN) {
        // Mở/Đóng Pause Menu bằng phím ESC
        if (event.key.scancode == SDL_SCANCODE_ESCAPE) {
            if (data.gameState == GameState::Playing) {
                data.gameState = GameState::Paused;
            } else if (data.gameState == GameState::Paused) {
                data.gameState = GameState::Playing;
            }
        }

        if (data.gameState == GameState::GameOver && event.key.scancode == SDL_SCANCODE_R) {
            MapSystem::resetGame(data);
            return;
        }
if (data.gameState == GameState::Playing) {
            if (event.key.scancode == SDL_SCANCODE_1) data.selectedTowerType = TowerType::Basic;
            else if (event.key.scancode == SDL_SCANCODE_2) data.selectedTowerType = TowerType::Frost;
            else if (event.key.scancode == SDL_SCANCODE_3) data.selectedTowerType = TowerType::Electric;
            else if (event.key.scancode == SDL_SCANCODE_4) data.selectedTowerType = TowerType::Cannon;
            else if (event.key.scancode == SDL_SCANCODE_5) data.selectedTowerType = TowerType::Tesla;
            // Tốc độ chơi
            else if (event.key.scancode == SDL_SCANCODE_MINUS) data.gameSpeed = 0.5f;
            else if (event.key.scancode == SDL_SCANCODE_EQUALS) data.gameSpeed = 1.0f;
            else if (event.key.scancode == SDL_SCANCODE_RIGHTBRACKET) data.gameSpeed = 1.5f;
            else if (event.key.scancode == SDL_SCANCODE_BACKSLASH) data.gameSpeed = 2.0f;        }
    }
if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_RIGHT) {
        int gridX = (int)(event.button.x / TILE_SIZE);
        int gridY = (int)(event.button.y / TILE_SIZE);
        for (auto it = data.operators.begin(); it != data.operators.end(); ++it) {
            if (it->pos.x == gridX && it->pos.y == gridY) {
                // Hoàn lại 50% tiền
                data.gold += getTowerConfig(it->type).cost / 2;
                data.operators.erase(it);
                break;
            }
        }
    }    
    // Nâng cấp tháp bằng middle-click
    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_MIDDLE) {
        int gridX = (int)(event.button.x / TILE_SIZE);
        int gridY = (int)(event.button.y / TILE_SIZE);
        for (auto& tower : data.operators) {
            if (tower.pos.x == gridX && tower.pos.y == gridY) {
                int upgradeCost = (tower.level) * getTowerConfig(tower.type).cost / 2;
                if (tower.level < 5 && data.gold >= upgradeCost) {
                    data.gold -= upgradeCost;
                    tower.level++;
                    tower.rangeBonus += 0.1f * TILE_SIZE;
                    tower.cooldownBonus = tower.level * 0.05f;
                }
                break;
            }
        }
    }    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        float mouseX = event.button.x;
        float mouseY = event.button.y;
// 1. XỬ LÝ CLICK: MAIN MENU (Giao diện lệch sang phải)
        if (data.gameState == GameState::MainMenu) {
        float panelW = 320.0f;
        float panelH = 380.0f;
        float panelX = (MAP_WIDTH * TILE_SIZE - panelW) / 2.0f; 
        float panelY = (MAP_HEIGHT * TILE_SIZE - panelH) / 2.0f;
            float btnW = 200.0f, btnH = 60.0f, gap = 30.0f;
            float btnX = panelX + (panelW - btnW) / 2.0f;
            float startBtnY = panelY + 80.0f;

            if (isInsideRect(mouseX, mouseY, btnX, startBtnY, btnW, btnH)) 
              MapSystem::resetGame(data, 1); // Click "Play"
            else if (isInsideRect(mouseX, mouseY, btnX, startBtnY + (btnH + gap), btnW, btnH)) 
                data.gameState = GameState::Settings;    // Click "Options"
            else if (isInsideRect(mouseX, mouseY, btnX, startBtnY + (btnH + gap) * 2, btnW, btnH)) 
                data.requestQuit = true;                 // Click "Quit"
            return;
        }

        // 3. XỬ LÝ CLICK: CÀI ĐẶT (SETTINGS)
        if (data.gameState == GameState::Settings) {
            int centerX = (MAP_WIDTH * TILE_SIZE) / 2;
            int centerY = (MAP_HEIGHT * TILE_SIZE) / 2;
            int btnW = 240, btnH = 50, gap = 20;

            if (isInsideRect(mouseX, mouseY, centerX - btnW/2, centerY - 20, btnW, btnH)) 
                data.soundEnabled = !data.soundEnabled; // Bật/Tắt âm thanh
            else if (isInsideRect(mouseX, mouseY, centerX - btnW/2, centerY - 20 + btnH + gap, btnW, btnH)) 
                data.gameState = GameState::MainMenu; // Nút BACK
            return;
        }

       // 4. XỬ LÝ CLICK: PAUSED & GAME OVER (Menu căn giữa như cũ)
        if (data.gameState == GameState::GameOver || data.gameState == GameState::Paused) {
            int centerX = (MAP_WIDTH * TILE_SIZE) / 2;
            int centerY = (MAP_HEIGHT * TILE_SIZE) / 2;
            int btnW = 240, btnH = 50;

            if (isInsideRect(mouseX, mouseY, centerX - btnW/2, centerY, btnW, btnH)) {
                if (data.gameState == GameState::GameOver) MapSystem::resetGame(data, data.currentLevel); 
                else if (data.gameState == GameState::Paused) data.gameState = GameState::Playing; 
            }
            return;
        }
if (mouseX >= 800 && mouseX <= 950 && mouseY >= 10 && mouseY <= 220) {
            // Kiểm tra click vào khoảng X của các nút
            if (mouseX >= 815 && mouseX <= 935) {
                // Tọa độ Y tương ứng với 5 nút
                if (mouseY >= 40 && mouseY <= 70) data.selectedTowerType = TowerType::Basic;
                else if (mouseY >= 75 && mouseY <= 105) data.selectedTowerType = TowerType::Frost;
                else if (mouseY >= 110 && mouseY <= 140) data.selectedTowerType = TowerType::Electric;
                else if (mouseY >= 145 && mouseY <= 175) data.selectedTowerType = TowerType::Cannon;
                else if (mouseY >= 180 && mouseY <= 210) data.selectedTowerType = TowerType::Tesla;
            }
            return; // Trả về luôn để không đặt tháp khi bấm vào giao diện
        }
        // LOGIC ĐẶT THÁP
        int gridX = (int)(mouseX / TILE_SIZE);
        int gridY = (int)(mouseY / TILE_SIZE);
        
        if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) return;

        int towerCost = getTowerConfig(data.selectedTowerType).cost;
        
        // SỬA LỖI 1: Thêm data.selectedTowerType vào hàm isBuildableTile
        if (data.gold >= towerCost && isBuildableTile(data, gridX, gridY, data.selectedTowerType) && !hasOperatorAt(data, gridX, gridY)) {
            if (data.operators.size() < data.maxTowers) {
                Operator newOp;
                newOp.pos = {gridX, gridY};
                newOp.type = data.selectedTowerType;
                
                // SỬA LỖI 3: Gán máu tối đa cho tháp (rất quan trọng cho tháp Blocker)
                newOp.hp = getTowerConfig(data.selectedTowerType).maxHp; 
                
                data.operators.push_back(newOp);
                data.gold -= towerCost;
            } else {
                std::cout << "Da dat gioi han thap (" << data.maxTowers << ")!" << std::endl;
            }
        }
    }
}