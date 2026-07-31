#include "LogicSystem.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace {
float toWorldCenterX(int gridX) {
    return (float)gridX * TILE_SIZE + TILE_SIZE * 0.5f;
}

float toWorldCenterY(int gridY) {
    return (float)gridY * TILE_SIZE + TILE_SIZE * 0.5f;
}

Enemy* findEnemyById(GameData& data, int enemyId) {
    for (auto& enemy : data.enemies) {
        if (enemy.id == enemyId && enemy.active) {
            return &enemy;
        }
    }
    return nullptr;
}

float distanceSquared(float x1, float y1, float x2, float y2) {
    float dx = x1 - x2;
    float dy = y1 - y2;
    return dx * dx + dy * dy;
}
}

void LogicSystem::update(GameData& data, float deltaTime) {
if (data.gameState != GameState::Playing) {
        return;
    }
    if (data.baseHP <= 0) {
        data.gameState = GameState::GameOver;
        return;
    }
    if (data.waveInProgress) {
        data.spawnTimer += deltaTime;
        if (data.enemiesSpawnedThisWave < data.enemiesPerWave && data.spawnTimer >= SPAWN_INTERVAL) {
            data.spawnTimer = 0.0f;
            Enemy enemy;
            enemy.id = data.nextEnemyId++;
            enemy.currentStep = 0;
            enemy.hp = 6 + (data.currentWave - 1) * 3;
            enemy.maxHp = enemy.hp;
            data.enemies.push_back(enemy);
            data.enemiesSpawnedThisWave++;
        }

        if (data.enemiesSpawnedThisWave >= data.enemiesPerWave && data.enemies.empty()) {
            data.waveInProgress = false;
            data.waveDelayTimer = 0.0f;
        }
    } else {
        data.waveDelayTimer += deltaTime;
        if (data.waveDelayTimer >= NEXT_WAVE_DELAY) {
            data.waveDelayTimer = 0.0f;
            data.waveInProgress = true;
            data.currentWave++;
            data.enemiesPerWave += 2;
            data.enemiesSpawnedThisWave = 0;
            data.spawnTimer = 0.0f;
        }
    }
// XỬ LÝ DI CHUYỂN CỦA QUÁI & BỊ CHẶN BỞI BLOCKER
for (auto& enemy : data.enemies) {
        if (!enemy.active) continue;

        // Giảm thời gian làm chậm
        if (enemy.slowTimer > 0.0f) enemy.slowTimer -= deltaTime;

        // Tính tốc độ di chuyển hiện tại
        float currentMoveInterval = ENEMY_MOVE_INTERVAL;
        if (enemy.slowTimer > 0.0f) currentMoveInterval *= 2.0f; // Bị làm chậm (chạy chậm đi một nửa)

        // Kiểm tra xem phía trước có tháp Blocker không
        bool isBlocked = false;
        Operator* blocker = nullptr;
        Position currentPos = data.enemyPath[enemy.currentStep];
        
        for (auto& op : data.operators) {
            if (op.type == TowerType::Blocker && op.pos.x == currentPos.x && op.pos.y == currentPos.y) {
                isBlocked = true;
                blocker = &op;
                break;
            }
        }
        if (isBlocked) {
            // Đứng lại cắn tháp chặn đường
            enemy.moveTimer = 0.0f; // Khởi tạo lại tiến trình di chuyển
            enemy.attackTimer += deltaTime;
            if (enemy.attackTimer >= 0.5f) { // Quái cắn tháp mỗi 0.5 giây
                enemy.attackTimer = 0.0f;
                if (blocker) {
                    blocker->hp -= 2;
                    if (blocker->hp <= 0) blocker->active = false;
                }
            }
        } else {
            // Di chuyển bình thường nếu không bị chặn
            enemy.moveTimer += deltaTime;
            if (enemy.moveTimer >= currentMoveInterval) {
                enemy.moveTimer = 0.0f;
                if (enemy.currentStep < (int)data.enemyPath.size() - 1) {
                    enemy.currentStep++;
                } else {
                    data.baseHP--;
                    if (data.baseHP >= 0 && data.baseHP < 3) data.lostHeartTime[data.baseHP] = SDL_GetTicks();
                    enemy.active = false;
                }
            }
        }
    }
// THÁP BẮN ĐẠN & GÂY SÁT THƯƠNG
for (auto& tower : data.operators) {
        if (!tower.active) continue;
        
        tower.fireTimer += deltaTime;
        TowerConfig stats = getTowerConfig(tower.type);
        if (tower.fireTimer < stats.cooldown) continue;

        // Nếu là tháp Blocker -> Đánh cận chiến quái dẫm lên nó
        if (tower.type == TowerType::Blocker) {
            for (auto& enemy : data.enemies) {
                if (!enemy.active) continue;
                Position enemyPos = data.enemyPath[enemy.currentStep];
                if (enemyPos.x == tower.pos.x && enemyPos.y == tower.pos.y) {
                    enemy.hp -= stats.damage;
                    if (enemy.hp <= 0) {
                        enemy.active = false;
                        data.gold += 10;
                    }
                    tower.fireTimer = 0.0f;
                    break; // Mỗi lần chỉ chọc 1 quái
                }
            }
            continue;
        }

        // Với các tháp Basic, Frost -> Bắn đạn
        const float towerX = toWorldCenterX(tower.pos.x);
        const float towerY = toWorldCenterY(tower.pos.y);
        Enemy* target = nullptr;
        float bestDistance = stats.range * stats.range;

        for (auto& enemy : data.enemies) {
            if (!enemy.active) continue;
            Position enemyTile = data.enemyPath[enemy.currentStep];
            float enemyX = toWorldCenterX(enemyTile.x);
            float enemyY = toWorldCenterY(enemyTile.y);
            float distSq = distanceSquared(towerX, towerY, enemyX, enemyY);
            if (distSq <= bestDistance) {
                bestDistance = distSq;
                target = &enemy;
            }
        }

        if (target != nullptr) {
            Projectile projectile;
            projectile.x = towerX;
            projectile.y = towerY;
            projectile.targetEnemyId = target->id;
            projectile.damage = stats.damage;
            projectile.speed = stats.projectileSpeed;
            projectile.splashRadius = stats.splashRadius;
            projectile.sourceType = tower.type;
            projectile.isFrost = stats.isFrost; // Gán thuộc tính băng
            data.projectiles.push_back(projectile);
            tower.fireTimer = 0.0f;
        }
    }
// DI CHUYỂN ĐẠN VÀ XỬ LÝ VA CHẠM
    for (auto& projectile : data.projectiles) {
        if (!projectile.active) continue;
        Enemy* target = findEnemyById(data, projectile.targetEnemyId);
        if (target == nullptr) { projectile.active = false; continue; }

        Position enemyTile = data.enemyPath[target->currentStep];
        float targetX = toWorldCenterX(enemyTile.x);
        float targetY = toWorldCenterY(enemyTile.y);
        float dx = targetX - projectile.x;
        float dy = targetY - projectile.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist <= projectile.speed * deltaTime || dist <= 1.0f) {
            // Khi bắn trúng
            if (projectile.isFrost) {
                target->slowTimer = 2.0f; // Áp dụng làm chậm 2 giây
            }
            target->hp -= projectile.damage;
            if (target->hp <= 0) {
                target->active = false;
                data.gold += 10;
            }
            projectile.active = false;
        } else {
            projectile.x += (dx / dist) * projectile.speed * deltaTime;
            projectile.y += (dy / dist) * projectile.speed * deltaTime;
        }
    }
    data.enemies.erase(std::remove_if(data.enemies.begin(), data.enemies.end(), [](const Enemy& e) { return !e.active; }), data.enemies.end());
    data.projectiles.erase(std::remove_if(data.projectiles.begin(), data.projectiles.end(), [](const Projectile& p) { return !p.active; }), data.projectiles.end());
    data.operators.erase(std::remove_if(data.operators.begin(), data.operators.end(), [](const Operator& op) { return !op.active; }), data.operators.end());
}