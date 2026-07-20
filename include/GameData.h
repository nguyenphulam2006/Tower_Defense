#pragma once
#include <vector>

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

enum class TowerType {
    Basic,
    Sniper,
    Splash
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

class GameData {
public:
    int baseHP = 3;
    bool gameOver = false;
    int hoveredGridX = -1;
    int hoveredGridY = -1;
    bool hoveredGridBuildable = false;
    std::vector<std::vector<TileType>> tileMap = {
        {TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass},
        {TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass},
        {TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Path,  TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass},
        {TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Path,  TileType::Base},
        {TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass, TileType::Grass}
    };
    std::vector<Position> enemyPath = {
        {0, 1}, {1, 1}, {2, 1}, {3, 1}, {4, 1}, 
        {5, 1}, {5, 2}, {5, 3}, {6, 3}, {7, 3}, 
        {8, 3}, {9, 3}, {10, 3}, {11, 3}, {12, 3}, {13, 3}
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

    int currentStep = 0;
    bool enemyActive = true;
    float moveTimer = 0.0f;

    void resetGame() {
        *this = GameData();
    }
};