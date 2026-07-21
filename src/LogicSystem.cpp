#include "LogicSystem.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace {
struct TowerStats {
    float range;
    float cooldown;
    int damage;
    float projectileSpeed;
    float splashRadius;
};

TowerStats getTowerStats(TowerType type) {
    switch (type) {
        case TowerType::Basic:
            return {TILE_SIZE * 2.5f, 0.75f, 1, 360.0f, 0.0f};
        case TowerType::Sniper:
            return {TILE_SIZE * 4.0f, 1.35f, 3, 520.0f, 0.0f};
        case TowerType::Splash:
            return {TILE_SIZE * 2.0f, 1.0f, 1, 300.0f, (float)TILE_SIZE * 1.0f};
    }

    return {TILE_SIZE * 2.5f, 0.75f, 1, 360.0f, 0.0f};
}

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
            enemy.hp = 3 + (data.currentWave - 1);
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

    for (auto& enemy : data.enemies) {
        if (!enemy.active) {
            continue;
        }

        enemy.moveTimer += deltaTime;
        if (enemy.moveTimer >= ENEMY_MOVE_INTERVAL) {
            enemy.moveTimer = 0.0f;
            if (enemy.currentStep < (int)data.enemyPath.size() - 1) {
                enemy.currentStep++;
            } else {
                data.baseHP--;
                if (data.baseHP >= 0 && data.baseHP < 3) {
                    data.lostHeartTime[data.baseHP] = SDL_GetTicks();
                }
                enemy.active = false;
                std::cout << "Canh bao: Quai da lot vao Base! Mau hien tai: " << data.baseHP << std::endl;
            }
        }
    }

    for (auto& tower : data.operators) {
        tower.fireTimer += deltaTime;
        TowerStats stats = getTowerStats(tower.type);
        if (tower.fireTimer < stats.cooldown) {
            continue;
        }

        const float towerX = toWorldCenterX(tower.pos.x);
        const float towerY = toWorldCenterY(tower.pos.y);

        Enemy* target = nullptr;
        float bestDistance = stats.range * stats.range;
        for (auto& enemy : data.enemies) {
            if (!enemy.active) {
                continue;
            }

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
            data.projectiles.push_back(projectile);
            tower.fireTimer = 0.0f;
        }
    }

    for (auto& projectile : data.projectiles) {
        if (!projectile.active) {
            continue;
        }

        Enemy* target = findEnemyById(data, projectile.targetEnemyId);
        if (target == nullptr) {
            projectile.active = false;
            continue;
        }

        Position enemyTile = data.enemyPath[target->currentStep];
        float targetX = toWorldCenterX(enemyTile.x);
        float targetY = toWorldCenterY(enemyTile.y);
        float dx = targetX - projectile.x;
        float dy = targetY - projectile.y;
        float dist = std::sqrt(dx * dx + dy * dy);

        if (dist <= projectile.speed * deltaTime || dist <= 1.0f) {
            if (projectile.splashRadius > 0.0f) {
                for (auto& enemy : data.enemies) {
                    if (!enemy.active) {
                        continue;
                    }

                    Position splashTile = data.enemyPath[enemy.currentStep];
                    float splashX = toWorldCenterX(splashTile.x);
                    float splashY = toWorldCenterY(splashTile.y);
                    float splashDistance = std::sqrt(distanceSquared(targetX, targetY, splashX, splashY));
                    if (splashDistance <= projectile.splashRadius) {
                        enemy.hp -= projectile.damage;
                        if (enemy.hp <= 0) {
                            enemy.active = false;
                            data.gold += 10;
                        }
                    }
                }
            } else {
                target->hp -= projectile.damage;
                if (target->hp <= 0) {
                    target->active = false;
                    data.gold += 10;
                }
            }
            projectile.active = false;
        } else {
            projectile.x += (dx / dist) * projectile.speed * deltaTime;
            projectile.y += (dy / dist) * projectile.speed * deltaTime;
        }
    }

    data.enemies.erase(
        std::remove_if(data.enemies.begin(), data.enemies.end(), [](const Enemy& enemy) {
            return !enemy.active;
        }),
        data.enemies.end());

    data.projectiles.erase(
        std::remove_if(data.projectiles.begin(), data.projectiles.end(), [](const Projectile& projectile) {
            return !projectile.active;
        }),
        data.projectiles.end());
}