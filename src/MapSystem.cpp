#include "MapSystem.h"
#include <iostream>
#include <fstream>
#include <sstream>

void MapSystem::initGame(GameData& data) {
    loadMapFromFile(data, "assets/Map/map1.txt");
    data.gameState = GameState::MainMenu;
}

void MapSystem::resetGame(GameData& data, int levelToLoad) {
    if (levelToLoad != -1) {
        data.currentLevel = levelToLoad;
    }
    loadMapFromFile(data, "assets/Map/map" + std::to_string(data.currentLevel) + ".txt");

    data.baseHP = 3;
    data.gold = 100;
    data.currentWave = 1;
    data.enemiesPerWave = 4 + data.currentLevel;
    data.enemiesSpawnedThisWave = 0;
    data.nextEnemyId = 1;
    data.spawnTimer = 0.0f;
    data.waveDelayTimer = 0.0f;
    data.waveInProgress = true;
    
    data.enemies.clear();
    data.operators.clear();
    data.projectiles.clear();
    
    data.lostHeartTime[0] = 0;
    data.lostHeartTime[1] = 0;
    data.lostHeartTime[2] = 0;
    
    data.gameState = GameState::Playing;
}

void MapSystem::loadMapFromFile(GameData& data, const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        file.open(std::string("../") + filepath); 
    }

    if (!file.is_open()) {
        std::cout << "Loi: Khong the mo file map: " << filepath << std::endl;
        return;
    }

    data.rawMap.clear();
    std::string line;
    
    while (std::getline(file, line)) {
        if (line.empty()) continue; 

        std::vector<int> row;
        std::istringstream ss(line);
        std::string token;
        
        while (ss >> token) {
            try {
                int tile = std::stoi(token);
                row.push_back(tile);
            } catch (...) {
                continue; 
            }
        }
        
        if (!row.empty()) {
            data.rawMap.push_back(row);
        }
    }
    file.close();
    loadAndGeneratePath(data);
}

void MapSystem::loadAndGeneratePath(GameData& data) {
    if (data.rawMap.empty() || data.rawMap.size() != MAP_HEIGHT || data.rawMap[0].size() != MAP_WIDTH) {
        std::cout << "Loi: Kich thuoc map khong khop voi MAP_WIDTH/HEIGHT (15x15)!" << std::endl;
        return;
    }

    data.tileMap.assign(MAP_HEIGHT, std::vector<TileType>(MAP_WIDTH));
    Position startPos = {-1, -1};

    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            data.tileMap[y][x] = static_cast<TileType>(data.rawMap[y][x]);
        }
    }

    int bestScore = INT_MAX;
    for (int y = 0; y < MAP_HEIGHT; ++y) {
        for (int x = 0; x < MAP_WIDTH; ++x) {
            if (data.tileMap[y][x] != TileType::Path) continue;

            int validNeighborCount = 0;
            for (int dir = 0; dir < 4; ++dir) {
                int nx = x + ((dir == 0) ? 1 : (dir == 1) ? -1 : 0);
                int ny = y + ((dir == 2) ? 1 : (dir == 3) ? -1 : 0);
                if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT) {
                    if (data.tileMap[ny][nx] == TileType::Path || data.tileMap[ny][nx] == TileType::Base) {
                        validNeighborCount++;
                    }
                }
            }

            int boundaryBias = (x == 0 || y == 0 || x == MAP_WIDTH - 1 || y == MAP_HEIGHT - 1) ? 0 : 10;
            int score = boundaryBias + validNeighborCount * 10 + x + y;

            if (validNeighborCount <= 2 && score < bestScore) {
                startPos = {x, y};
                bestScore = score;
            }
        }
    }

    if (startPos.x == -1) {
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                if (data.tileMap[y][x] == TileType::Path) {
                    startPos = {x, y};
                    break;
                }
            }
            if (startPos.x != -1) break;
        }
    }

    data.enemyPath.clear();
    if (startPos.x == -1) {
        std::cout << "Loi: Khong tim thay diem bat dau cua duong di!" << std::endl;
        return;
    }

    std::vector<std::vector<bool>> visited(MAP_HEIGHT, std::vector<bool>(MAP_WIDTH, false));
    Position current = startPos;
    data.enemyPath.push_back(current);
    visited[current.y][current.x] = true;

    int dx[] = {1, -1, 0, 0};
    int dy[] = {0, 0, 1, -1};

    bool reachedBase = false;
    while (!reachedBase) {
        bool moved = false;
        for (int i = 0; i < 4; ++i) {
            int nx = current.x + dx[i];
            int ny = current.y + dy[i];

            if (nx >= 0 && nx < MAP_WIDTH && ny >= 0 && ny < MAP_HEIGHT && !visited[ny][nx]) {
                if (data.tileMap[ny][nx] == TileType::Path) {
                    current = {nx, ny};
                    data.enemyPath.push_back(current);
                    visited[ny][nx] = true;
                    moved = true;
                    break;
                }
                else if (data.tileMap[ny][nx] == TileType::Base) {
                    current = {nx, ny};
                    data.enemyPath.push_back(current);
                    reachedBase = true;
                    moved = true;
                    break;
                }
            }
        }
        if (!moved) break;
    }
}