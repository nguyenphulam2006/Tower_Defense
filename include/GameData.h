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

enum class TowerType { Basic, Frost, Electric, Cannon, Tesla };
enum class GameState { MainMenu, LevelSelect, Settings, Playing, Paused, GameOver };
enum class TileType { Grass = 0, Path = 1, Base = 2, Water = 3, Lava = 4 };
enum class GameMode { Normal, Endless, Challenge };
enum class EnemyType {
    Normal, Fast, Tanker, Boss, 
    Goblin, Demon, Ghost, Zombie, Skeleton, Bat, KingSlime, BigSlime
};
struct Position { int x, y; };
struct Operator { 
    Position pos; 
    TowerType type = TowerType::Basic; 
    float fireTimer = 0.0f; 
    int hp = 5;
    bool active = true;
    int level = 1;               // Cấp độ tháp (1-5)
    int totalDamage = 0;         // Tổng sát thương gây ra
    float rangeBonus = 0.0f;     // Bonus range từ nâng cấp
    float cooldownBonus = 0.0f;  // Bonus cooldown
};
struct EnemyConfig {
    float baseHp;
    float speedMultiplier; // Hệ số tốc độ (nhỏ hơn 1 là nhanh hơn)
    int reward;            // Tiền thưởng khi giết được
};
struct Enemy {
    int id;
    int currentStep;
    float hp;
    float maxHp;
    bool active = true;
    float slowTimer = 0.0f;
    float moveTimer = 0.0f;
    float attackTimer = 0.0f;
    EnemyType type = EnemyType::Normal;
    float speedMultiplier = 1.0f;
    int reward = 10;
};
struct Projectile { 
    float x = 0.0f; float y = 0.0f; int targetEnemyId = -1; 
    int damage = 1; float speed = 360.0f; float splashRadius = 0.0f; 
    TowerType sourceType = TowerType::Basic; bool active = true; 
    bool isFrost = false;
    bool isElectric = false;
    int chainCount = 0;  // Số lần đã xích tụ
};

struct Particle {
    float x = 0.0f, y = 0.0f;
    float vx = 0.0f, vy = 0.0f;
    float lifetime = 0.0f;
    float maxLifetime = 1.0f;
    int r = 255, g = 255, b = 255;
    float scale = 1.0f;
    bool active = true;
};

struct VisualEffect {
    float x = 0.0f, y = 0.0f;
    float lifetime = 0.0f;
    float maxLifetime = 0.5f;
    int type = 0;  // 0=explosion, 1=spark, 2=heal
    bool active = true;
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
        // Tháp điện: Xích tụ giữa các quái, sát thương cao
        case TowerType::Electric: return {"Electric", 45, TILE_SIZE * 3.0f, 1.0f, 2, 400.0f, 0.0f, false, 1};
        // Tháp pháo: Sát thương AOE cao, cooldown dài
        case TowerType::Cannon: return {"Cannon", 55, TILE_SIZE * 2.0f, 1.8f, 4, 250.0f, TILE_SIZE * 1.2f, false, 1};
        // Tháp Tesla: Bắn liên tục khi quái gần, tăng damage theo thời gian
        case TowerType::Tesla: return {"Tesla", 50, TILE_SIZE * 2.0f, 0.3f, 1, 0.0f, 0.0f, false, 1};
    }
    return {"Basic", 25, TILE_SIZE * 2.5f, 0.75f, 1, 360.0f, 0.0f, false, 1};
}
inline EnemyConfig getEnemyConfig(EnemyType type) {
    switch(type) {
        case EnemyType::Fast:       return {4.0f, 0.6f, 5};
        case EnemyType::Tanker:     return {20.0f, 1.5f, 15};
        case EnemyType::Boss:       return {80.0f, 1.3f, 50};
        case EnemyType::Goblin:     return {6.0f, 0.9f, 8};
        case EnemyType::Demon:      return {12.0f, 1.1f, 12};
        case EnemyType::Ghost:      return {5.0f, 0.6f, 10};
        case EnemyType::Zombie:     return {25.0f, 1.6f, 15};
        case EnemyType::Skeleton:   return {8.0f, 1.0f, 8};
        case EnemyType::Bat:        return {3.0f, 0.5f, 6};
        case EnemyType::BigSlime:   return {30.0f, 1.4f, 20};
        case EnemyType::KingSlime:  return {100.0f, 1.2f, 100};
        
        case EnemyType::Normal: 
        default:                    return {8.0f, 1.0f, 10};
    }
}
class GameData {
public:
    // === GAME STATE ===
    int baseHP = 3;
    Uint64 lostHeartTime[3] = {0, 0, 0};
    GameState gameState = GameState::MainMenu;
    bool requestQuit = false;
    int hoveredGridX = -1;
    int hoveredGridY = -1;
    bool hoveredGridBuildable = false;
    
    // === SETTINGS ===
    int maxTowers = 5;
    int currentLevel = 1;
    bool soundEnabled = true;
    bool showGrid = false;
    float gameSpeed = 1.0f;  // 0.5x, 1.0x, 1.5x, 2.0x
    GameMode gameMode = GameMode::Normal;
    
    // === MAP & PATH ===
    std::vector<std::vector<int>> rawMap;
    std::vector<std::vector<TileType>> tileMap;
    std::vector<Position> enemyPath;

    // === ENTITIES ===
    std::vector<Enemy> enemies;
    std::vector<Operator> operators;
    std::vector<Projectile> projectiles;
    std::vector<Particle> particles;
    std::vector<VisualEffect> effects;

    // === RESOURCES ===
    int gold = 100;
    int totalGoldEarned = 0;  // Thống kê
    int highScore = 0;
    TowerType selectedTowerType = TowerType::Basic;
    
    // === WAVE SYSTEM ===
    int currentWave = 1;
    int enemiesPerWave = 5;
    int enemiesSpawnedThisWave = 0;
    int nextEnemyId = 1;
    int totalEnemiesKilled = 0;  // Thống kê
    int totalTowersBuilt = 0;    // Thống kê

    float spawnTimer = 0.0f;
    float waveDelayTimer = 0.0f;
    bool waveInProgress = true;
};