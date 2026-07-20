#include "InputSystem.h"
#include <iostream>

namespace {
int getTowerCost(TowerType type) {
    switch (type) {
        case TowerType::Basic:
            return 25;
        case TowerType::Sniper:
            return 40;
        case TowerType::Splash:
            return 35;
    }

    return 25;
}

const char* getTowerTypeName(TowerType type) {
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
    if (event.type == SDL_EVENT_MOUSE_MOTION) {
        int gridX = (int)(event.motion.x / TILE_SIZE);
        int gridY = (int)(event.motion.y / TILE_SIZE);
        data.hoveredGridX = gridX;
        data.hoveredGridY = gridY;
        data.hoveredGridBuildable = gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT && data.tileMap[gridY][gridX] == TileType::Grass;
    }

    if (event.type == SDL_EVENT_KEY_DOWN) {
        if (data.gameOver && event.key.scancode == SDL_SCANCODE_R) {
            data.resetGame();
            data.hoveredGridX = -1;
            data.hoveredGridY = -1;
            data.hoveredGridBuildable = false;
            std::cout << "Restart game\n";
            return;
        }

        if (event.key.scancode == SDL_SCANCODE_1) {
            data.selectedTowerType = TowerType::Basic;
            std::cout << "Chon tower: Basic\n";
        } else if (event.key.scancode == SDL_SCANCODE_2) {
            data.selectedTowerType = TowerType::Sniper;
            std::cout << "Chon tower: Sniper\n";
        } else if (event.key.scancode == SDL_SCANCODE_3) {
            data.selectedTowerType = TowerType::Splash;
            std::cout << "Chon tower: Splash\n";
        }
    }

    if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
            if (data.gameOver) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;
                if (isInsideRect(mouseX, mouseY, RETRY_BUTTON_X, RETRY_BUTTON_Y, RETRY_BUTTON_WIDTH, RETRY_BUTTON_HEIGHT)) {
                    data.resetGame();
                    data.hoveredGridX = -1;
                    data.hoveredGridY = -1;
                    data.hoveredGridBuildable = false;
                    std::cout << "Restart game\n";
                }
                return;
            }

            // Lấy tọa độ chuột
            float mouseX = event.button.x;
            float mouseY = event.button.y;

            // Chuyển pixel thành tọa độ Grid
            int gridX = (int)(mouseX / TILE_SIZE);
            int gridY = (int)(mouseY / TILE_SIZE);
            data.hoveredGridX = gridX;
            data.hoveredGridY = gridY;
            data.hoveredGridBuildable = gridX >= 0 && gridX < MAP_WIDTH && gridY >= 0 && gridY < MAP_HEIGHT && data.tileMap[gridY][gridX] == TileType::Grass;

            if (gridX < 0 || gridX >= MAP_WIDTH || gridY < 0 || gridY >= MAP_HEIGHT) {
                return;
            }

            int towerCost = getTowerCost(data.selectedTowerType);
            if (data.gold < towerCost) {
                std::cout << "Khong du vang de dat tower\n";
                return;
            }

            if (!isBuildableTile(data, gridX, gridY)) {
                std::cout << "Khong the dat tower len duong di\n";
                return;
            }

            if (hasOperatorAt(data, gridX, gridY)) {
                std::cout << "Da co tower o vi tri nay\n";
                return;
            }

            // Đặt Operator mới vào danh sách
            Operator newOp;
            newOp.pos = {gridX, gridY};
            newOp.type = data.selectedTowerType;
            data.operators.push_back(newOp);
            data.gold -= towerCost;
            
            std::cout << "Dat " << getTowerTypeName(newOp.type) << " tai o: (" << gridX << ", " << gridY << ")\n";
        }
    }
}