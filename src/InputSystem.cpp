#include "InputSystem.h"
#include <iostream>

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
        if (op.pos.x == gridX && op.pos.y == gridY) {
            return true;
        }
    }
    return false;
}


bool isInsideRect(int x, int y, int rectX, int rectY, int rectW, int rectH) {
    return x >= rectX && x < rectX + rectW && y >= rectY && y < rectY + rectH;
}

bool isBuildableTile(const GameData& data, int gridX, int gridY) {
    return data.tileMap[gridY][gridX] == TileType::Grass;
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
            data.resetGame();
            return;
        }

        if (data.gameState == GameState::Playing) {
            if (event.key.scancode == SDL_SCANCODE_1) data.selectedTowerType = TowerType::Basic;
            else if (event.key.scancode == SDL_SCANCODE_2) data.selectedTowerType = TowerType::Sniper;
            else if (event.key.scancode == SDL_SCANCODE_3) data.selectedTowerType = TowerType::Splash;
        }
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button == SDL_BUTTON_LEFT) {
        float mouseX = event.button.x;
        float mouseY = event.button.y;
// 1. XỬ LÝ CLICK: MAIN MENU (Giao diện lệch sang phải)
        if (data.gameState == GameState::MainMenu) {
            float panelW = 320.0f;
            float panelX = (MAP_WIDTH * TILE_SIZE) - panelW - 100.0f;
            float panelY = 200.0f;
            
            float btnW = 200.0f, btnH = 60.0f, gap = 30.0f;
            float btnX = panelX + (panelW - btnW) / 2.0f;
            float startBtnY = panelY + 80.0f;

            if (isInsideRect(mouseX, mouseY, btnX, startBtnY, btnW, btnH)) 
                data.gameState = GameState::LevelSelect; // Click "Play"
            else if (isInsideRect(mouseX, mouseY, btnX, startBtnY + (btnH + gap), btnW, btnH)) 
                data.gameState = GameState::Settings;    // Click "Options"
            else if (isInsideRect(mouseX, mouseY, btnX, startBtnY + (btnH + gap) * 2, btnW, btnH)) 
                data.requestQuit = true;                 // Click "Quit"
            return;
        }

        // 2. XỬ LÝ CLICK: CHỌN MÀN CHƠI
        if (data.gameState == GameState::LevelSelect) {
            int centerX = (MAP_WIDTH * TILE_SIZE) / 2;
            int centerY = (MAP_HEIGHT * TILE_SIZE) / 2;
            int btnW = 240, btnH = 50, gap = 20;

            if (isInsideRect(mouseX, mouseY, centerX - btnW/2, centerY - 60, btnW, btnH)) 
                data.resetGame(1); // Chơi Level 1
            else if (isInsideRect(mouseX, mouseY, centerX - btnW/2, centerY - 60 + btnH + gap, btnW, btnH)) 
                data.resetGame(2); // Chơi Level 2
            else if (isInsideRect(mouseX, mouseY, centerX - btnW/2, centerY - 60 + (btnH + gap)*2, btnW, btnH)) 
                data.gameState = GameState::MainMenu; // Nút BACK
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
                if (data.gameState == GameState::GameOver) data.resetGame(data.currentLevel); 
                else if (data.gameState == GameState::Paused) data.gameState = GameState::Playing; 
            }
            return;
        }
        int gridX = (int)(mouseX / TILE_SIZE);
        int gridY = (int)(mouseY / TILE_SIZE);
        
        if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) return;

       int towerCost = getTowerConfig(data.selectedTowerType).cost;
        if (data.gold >= towerCost && isBuildableTile(data, gridX, gridY) && !hasOperatorAt(data, gridX, gridY)) {
            Operator newOp;
            newOp.pos = {gridX, gridY};
            newOp.type = data.selectedTowerType;
            data.operators.push_back(newOp);
            data.gold -= towerCost;
        }
    }
}