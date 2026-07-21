#pragma once
#include <vector>
#include <SDL3/SDL.h>
const int TILE_SIZE = 64;
const int MAP_WIDTH = 15;
const int MAP_HEIGHT = 5;
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

enum class TowerType {
    Basic,
    Sniper,
    Splash
};

enum class GameState {
    MainMenu,
    Playing,
    Paused,
    GameOver
};

enum class TileType {
    Grass,
    Path,
    Base
};

struct Position {
    int x, y;
};
struct Operator {
    Position pos;
    TowerType type = TowerType::Basic;
    float fireTimer = 0.0f;
};

struct Enemy {
    int id = 0;
    int currentStep = 0;
    int hp = 3;
    float moveTimer = 0.0f;
    bool active = true;
};

struct Projectile {
    float x = 0.0f;
    float y = 0.0f;
    int targetEnemyId = -1;
    int damage = 1;
    float speed = 360.0f;
    float splashRadius = 0.0f;
    TowerType sourceType = TowerType::Basic;
    bool active = true;
};
struct TowerConfig {
    const char* name;
    int cost;
    float range;
    float cooldown;
    int damage;
    float projectileSpeed;
    float splashRadius;
};
inline TowerConfig getTowerConfig(TowerType type) {
    switch (type) {
        case TowerType::Basic:
            return {"Basic", 25, TILE_SIZE * 2.5f, 0.75f, 1, 360.0f, 0.0f};
        case TowerType::Sniper:
            return {"Sniper", 40, TILE_SIZE * 4.0f, 1.35f, 3, 520.0f, 0.0f};
        case TowerType::Splash:
            return {"Splash", 35, TILE_SIZE * 2.0f, 1.0f, 1, 300.0f, (float)TILE_SIZE * 1.0f};
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
    std::vector<std::vector<TileType>> tileMap = {
    {TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass},
    {TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Base},
    {TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Path,  TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass},
    {TileType::Grass, TileType::Grass, TileType::Grass, TileType::Path,  TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Path,  TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass},
    {TileType::Grass, TileType::Grass, TileType::Grass, TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass}
};
 std::vector<Position> enemyPath = {
    {0, 2}, {1, 2}, {2, 2}, {3, 2}, {3, 3}, {3, 4}, {4, 4}, {5, 4}, 
    {6, 4}, {7, 4}, {8, 4}, {9, 4}, {9, 3}, {9, 2}, {9, 1}, {10, 1}, 
    {11, 1}, {12, 1}, {13, 1}
};
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
 void resetGame() {
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