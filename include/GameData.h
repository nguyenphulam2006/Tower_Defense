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

enum class TowerType { Basic, Frost, Blocker };
enum class GameState { MainMenu, LevelSelect, Settings, Playing, Paused, GameOver };
enum class TileType { Grass = 0, Path = 1, Base = 2, Water = 3, Lava = 4 };

struct Position { int x, y; };
struct Operator { 
    Position pos; 
    TowerType type = TowerType::Basic; 
    float fireTimer = 0.0f; 
    int hp = 5; // THÊM: Máu cho tháp chặn đường
    bool active = true; // THÊM: Trạng thái tồn tại của tháp
};
struct Enemy { 
    int id = 0; int currentStep = 0; int hp = 5; int maxHp = 5; 
    float moveTimer = 0.0f; 
    bool active = true; 
    float slowTimer = 0.0f; // THÊM: Thời gian bị làm chậm
    float attackTimer = 0.0f; // THÊM: Cooldown đánh tháp chặn
};
struct Projectile { 
    float x = 0.0f; float y = 0.0f; int targetEnemyId = -1; 
    int damage = 1; float speed = 360.0f; float splashRadius = 0.0f; 
    TowerType sourceType = TowerType::Basic; bool active = true; 
    bool isFrost = false; 
};
struct TowerConfig { 
    const char* name; int cost; float range; float cooldown; 
    int damage; float projectileSpeed; float splashRadius; 
    bool isFrost; int maxHp; 
};
inline TowerConfig getTowerConfig(TowerType type) {
    switch (type) {
        // Tháp thường: Bắn đạn bình thường
        case TowerType::Basic: return {"Basic", 25, TILE_SIZE * 2.5f, 0.75f, 1, 360.0f, 0.0f, false, 1};
        // Tháp băng: Sát thương thấp, làm chậm địch
        case TowerType::Frost: return {"Frost", 35, TILE_SIZE * 2.5f, 1.2f, 1, 300.0f, 0.0f, true, 1};
        // Tháp chặn: Đặt trên đường đi, chặn quái, gây sát thương cận chiến, máu trâu
        case TowerType::Blocker: return {"Blocker", 20, TILE_SIZE * 0.5f, 1.0f, 1, 0.0f, 0.0f, false, 5};
    }
    return {"Basic", 25, TILE_SIZE * 2.5f, 0.75f, 1, 360.0f, 0.0f, false, 1};
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
    
    int maxTowers = 5;
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
};