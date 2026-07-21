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

        if (data.gameState == GameState::MainMenu || data.gameState == GameState::Paused || data.gameState == GameState::GameOver) {
            int btnX = (MAP_WIDTH * TILE_SIZE - MENU_BUTTON_WIDTH) / 2;
            int btnY = (MAP_HEIGHT * TILE_SIZE - MENU_BUTTON_HEIGHT) / 2;

            if (isInsideRect(mouseX, mouseY, btnX, btnY, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT)) {
                if (data.gameState == GameState::MainMenu || data.gameState == GameState::GameOver) {
                    data.resetGame();
                } else if (data.gameState == GameState::Paused) {
                    data.gameState = GameState::Playing;
                }
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