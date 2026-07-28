#pragma once
#include <vector>
#include <SDL3/SDL.h>
#include <iostream>
#include <fstream>   
#include <sstream>
#include <string>
const int TILE_SIZE = 64;
const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 15;
const int TOWER_COST = 25;

const float ENEMY_MOVE_INTERVAL = 0.5f;
const float TOWER_RANGE = TILE_SIZE * 2.5f;
const float TOWER_COOLDOWN = 0.75f;
const float PROJECTILE_SPEED = 360.0f;
const float SPAWN_INTERVAL = 1.0f;
const float NEXT_WAVE_DELAY = 2.0f;

const int RETRY_BUTTON_WIDTH = 200;
const int RETRY_BUTTON_HEIGHT = 56;
const int RETRY_BUTTON_X = (MAP_WIDTH * TILE_SIZE - RETRY_BUTTON_WIDTH) / 2;
const int RETRY_BUTTON_Y = (MAP_HEIGHT * TILE_SIZE - RETRY_BUTTON_HEIGHT) / 2 + 36;
const int MENU_PANEL_WIDTH = 380;
const int MENU_PANEL_HEIGHT = 280;
const int MENU_BUTTON_WIDTH = 240;
const int MENU_BUTTON_HEIGHT = 52;
const int MENU_BUTTON_GAP = 14;

enum class TowerType { Basic, Sniper, Splash };
enum class GameState { MainMenu, LevelSelect, Settings, Playing, Paused, GameOver };
enum class TileType { Grass = 0, Path = 1, Base = 2, Water = 3, Lava = 4 };

struct Position { int x, y; };
struct Operator { Position pos; TowerType type = TowerType::Basic; float fireTimer = 0.0f; };
struct Enemy { int id = 0; int currentStep = 0; int hp = 3; int maxHp = 3;; float moveTimer = 0.0f; bool active = true; };
struct Projectile { float x = 0.0f; float y = 0.0f; int targetEnemyId = -1; int damage = 1; float speed = 360.0f; float splashRadius = 0.0f; TowerType sourceType = TowerType::Basic; bool active = true; };
struct TowerConfig { const char* name; int cost; float range; float cooldown; int damage; float projectileSpeed; float splashRadius; };

inline TowerConfig getTowerConfig(TowerType type) {
    switch (type) {
        case TowerType::Basic: return {"Basic", 25, TILE_SIZE * 2.5f, 0.75f, 1, 360.0f, 0.0f};
        case TowerType::Sniper: return {"Sniper", 40, TILE_SIZE * 4.0f, 1.35f, 3, 520.0f, 0.0f};
        case TowerType::Splash: return {"Splash", 35, TILE_SIZE * 2.0f, 1.0f, 1, 300.0f, (float)TILE_SIZE * 1.0f};
    }
    return {"Basic", 25, TILE_SIZE * 2.5f, 0.75f, 1, 360.0f, 0.0f};
}
class GameData {
public:
    int baseHP = 3;
    Uint64 lostHeartTime[3] = {0, 0, 0};
    GameState gameState = GameState::MainMenu;
    bool requestQuit = false;
    int hoveredGridX = -1;
    int hoveredGridY = -1;
    bool hoveredGridBuildable = false;
    int maxTowers = 15; 
    int currentLevel = 1;
    bool soundEnabled = true;
    bool showGrid = false;
    std::vector<std::vector<int>> rawMap;
    std::vector<std::vector<TileType>> tileMap;
    std::vector<Position> enemyPath;

    std::vector<Enemy> enemies;
    std::vector<Operator> operators;
    std::vector<Projectile> projectiles;

    int gold = 100;
    TowerType selectedTowerType = TowerType::Basic;
    int currentWave = 1;
    int enemiesPerWave = 5;
    int enemiesSpawnedThisWave = 0;
    int nextEnemyId = 1;

    float spawnTimer = 0.0f;
    float waveDelayTimer = 0.0f;
    bool waveInProgress = true;

    // Constructor tự động khởi tạo map và dò đường
    GameData() {
        loadMapFromFile("assets/map1.txt");
    }
void loadMapFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            file.open(std::string("../") + filepath); 
        }

        if (!file.is_open()) {
            std::cout << "Loi: Khong the mo file map: " << filepath << std::endl;
            return;
        }

        rawMap.clear();
        std::string line;
        
        while (std::getline(file, line)) {
            if (line.empty()) continue; 

            std::vector<int> row;
            std::istringstream ss(line);
            std::string token;
            
            // Đọc dưới dạng chuỗi trước để loại bỏ các ký tự rác (như BOM)
            while (ss >> token) {
                try {
                    // Cố gắng chuyển chuỗi thành số nguyên
                    int tile = std::stoi(token);
                    row.push_back(tile);
                } catch (...) {
                    // Bỏ qua nếu gặp ký tự không phải là số
                    continue; 
                }
            }
            
            if (!row.empty()) {
                rawMap.push_back(row);
            }
        }
        file.close();
        loadAndGeneratePath();
    }
 void loadAndGeneratePath() {
        // Bổ sung kiểm tra an toàn: Đảm bảo map load lên đúng kích thước 15x15
        if (rawMap.empty() || rawMap.size() != MAP_HEIGHT || rawMap[0].size() != MAP_WIDTH) {
            std::cout << "Loi: Kich thuoc map trong file txt khong khop voi MAP_WIDTH/HEIGHT (15x15)!" << std::endl;
            return;
        }

        tileMap.resize(MAP_HEIGHT, std::vector<TileType>(MAP_WIDTH));
        Position startPos = {-1, -1};

        // 1. Chuyển đổi mảng số (rawMap) sang mảng TileType (tileMap)
        for (int y = 0; y < MAP_HEIGHT; ++y) {
            for (int x = 0; x < MAP_WIDTH; ++x) {
                tileMap[y][x] = static_cast<TileType>(rawMap[y][x]);
                
                // Điểm bắt đầu (ở lề trái x = 0)
                if (x == 0 && tileMap[y][x] == TileType::Path) {
                    startPos = {x, y};
                }
            }
        }

        // 2. Thuật toán tự động dò đường đi (Giữ nguyên như trước)
        enemyPath.clear();
        if (startPos.x == -1) {
            std::cout << "Loi: Khong tim thay diem bat dau cua duong di (Path) o cot 0!" << std::endl;
            return;
        }
        std::vector<std::vector<bool>> visited(MAP_HEIGHT, std::vector<bool>(MAP_WIDTH, false));
        Position current = startPos;
        enemyPath.push_back(current);
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
                    if (tileMap[ny][nx] == TileType::Path) {
                        current = {nx, ny};
                        enemyPath.push_back(current);
                        visited[ny][nx] = true;
                        moved = true;
                        break;
                    } 
                    else if (tileMap[ny][nx] == TileType::Base) {
                        current = {nx, ny};
                        enemyPath.push_back(current);
                        reachedBase = true;
                        moved = true;
                        break; 
                    }
                }
            }
            if (!moved) break;
        }
    }
void resetGame(int levelToLoad = -1) {
        if (levelToLoad != -1) {
            currentLevel = levelToLoad;
        }
        loadMapFromFile("assets/map" + std::to_string(currentLevel) + ".txt");

        baseHP = 3;
        gold = 100;
        currentWave = 1;
        enemiesPerWave = 5;
        enemiesSpawnedThisWave = 0;
        nextEnemyId = 1;
        spawnTimer = 0.0f;
        waveDelayTimer = 0.0f;
        waveInProgress = true;
        enemies.clear();
        operators.clear();
        projectiles.clear();
        lostHeartTime[0] = 0;
        lostHeartTime[1] = 0;
        lostHeartTime[2] = 0;
        
        this->gameState = GameState::Playing;
    }
};