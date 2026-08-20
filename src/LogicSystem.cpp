#include "LogicSystem.h"
#include "EffectsSystem.h"
#include "MapSystem.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <random>

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

float distance(float x1, float y1, float x2, float y2) {
    return std::sqrt(distanceSquared(x1, y1, x2, y2));
}

// Hàm lấy tất cả enemies trong range
std::vector<Enemy*> getEnemiesInRange(GameData& data, float centerX, float centerY, float rangeRadius) {
    std::vector<Enemy*> result;
    for (auto& enemy : data.enemies) {
        if (!enemy.active) continue;
        Position enemyPos = data.enemyPath[enemy.currentStep];
        float eX = toWorldCenterX(enemyPos.x);
        float eY = toWorldCenterY(enemyPos.y);
        if (distanceSquared(centerX, centerY, eX, eY) <= rangeRadius * rangeRadius) {
            result.push_back(&enemy);
        }
    }
    return result;
}
}

void LogicSystem::update(GameData& data, float deltaTime) {
    if (data.gameState != GameState::Playing) {
        return;
    }
    
    // Áp dụng tốc độ chơi
    deltaTime *= data.gameSpeed;
    
    if (data.baseHP <= 0) {
        data.gameState = GameState::GameOver;
        return;
    }

    // === WAVE SYSTEM ===
    if (data.waveInProgress) {
        data.spawnTimer += deltaTime;
        
        // Sinh quái mới
        if (data.enemiesSpawnedThisWave < data.enemiesPerWave && data.spawnTimer >= SPAWN_INTERVAL) {
            data.spawnTimer = 0.0f;
            Enemy enemy;
            enemy.id = data.nextEnemyId++;
            enemy.currentStep = 0;
            
            // Chọn loại quái dựa trên wave
            EnemyType type = EnemyType::Normal;
            
            if (data.currentWave % 4 == 0 && data.enemiesSpawnedThisWave == data.enemiesPerWave - 1) {
                type = EnemyType::KingSlime; 
            } 
            else {
                int randVal = std::rand() % 100;
                
                if (data.currentWave >= 6) {
                    // Wave rất khó: Tất cả quái mạnh
                    if (randVal < 25) type = EnemyType::Demon;
                    else if (randVal < 50) type = EnemyType::BigSlime;
                    else if (randVal < 75) type = EnemyType::Zombie;
                    else type = EnemyType::KingSlime;
                }
                else if (data.currentWave >= 4) {
                    if (randVal < 15) type = EnemyType::Demon;
                    else if (randVal < 30) type = EnemyType::BigSlime;
                    else if (randVal < 55) type = EnemyType::Ghost;
                    else if (randVal < 75) type = EnemyType::Zombie;
                    else if (randVal < 90) type = EnemyType::Skeleton;
                    else type = EnemyType::Bat;
                } 
                else if (data.currentWave >= 2) {
                    if (randVal < 25) type = EnemyType::Goblin;
                    else if (randVal < 50) type = EnemyType::Ghost;
                    else if (randVal < 75) type = EnemyType::Bat;
                    else if (randVal < 90) type = EnemyType::Skeleton;
                    else type = EnemyType::Zombie;
                } 
                else {
                    if (randVal < 50) type = EnemyType::Normal;
                    else if (randVal < 80) type = EnemyType::Goblin;
                    else type = EnemyType::Bat;
                }
            }

            EnemyConfig cfg = getEnemyConfig(type);
            enemy.type = type;
            enemy.speedMultiplier = cfg.speedMultiplier;
            enemy.reward = cfg.reward;
            
            // Scale HP theo wave (tăng 20% mỗi wave)
            float waveHpScale = 1.0f + (data.currentWave - 1) * 0.2f + (data.currentLevel - 1) * 0.35f;
            // Thêm bonus cho Endless mode
            if (data.gameMode == GameMode::Endless) {
                waveHpScale *= 1.0f + (data.currentWave - 1) * 0.1f;
            }

            enemy.hp = cfg.baseHp * waveHpScale;
            enemy.maxHp = enemy.hp;

            data.enemies.push_back(enemy);
            data.enemiesSpawnedThisWave++;
        }

        // Kiểm tra kết thúc wave
        if (data.enemiesSpawnedThisWave >= data.enemiesPerWave && data.enemies.empty()) {
            data.waveInProgress = false;
            data.waveDelayTimer = 0.0f;
            
            // Tính thưởng (có bonus theo Endless mode)
            int waveReward = 20 + (data.currentWave * 5);
            if (data.gameMode == GameMode::Endless) {
                waveReward = 30 + (data.currentWave * 8);
            }
            data.gold += waveReward;
            data.totalGoldEarned += waveReward;

            if (data.gameMode == GameMode::Normal && data.currentWave % 4 == 0) {
                if (data.currentLevel < 3) {
                    MapSystem::resetGame(data, data.currentLevel + 1);
                } else {
                    data.currentLevel = 1;
                    data.gameState = GameState::MainMenu;
                }
            }
        }
    } 
    else {
        // Chuyển sang wave tiếp theo
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

    // === MOVEMENT ===
    for (auto& enemy : data.enemies) {
        if (!enemy.active) continue;

        if (enemy.slowTimer > 0.0f) enemy.slowTimer -= deltaTime;

        float currentMoveInterval = ENEMY_MOVE_INTERVAL * enemy.speedMultiplier;
        if (enemy.slowTimer > 0.0f) currentMoveInterval *= 2.0f; 

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

    // === TOWER ACTIONS ===
    for (auto& tower : data.operators) {
        if (!tower.active) continue;
        
        tower.fireTimer += deltaTime;
        TowerConfig stats = getTowerConfig(tower.type);
        
        // Áp dụng bonus từ level
        float effectiveCooldown = stats.cooldown * (1.0f - tower.level * 0.05f);
        float effectiveRange = stats.range + tower.rangeBonus;
        int effectiveDamage = stats.damage + (tower.level - 1);
        
        if (tower.fireTimer < effectiveCooldown) continue;

        // TẤT CẢ các tháp giờ đây đều dùng chung logic tìm mục tiêu gần nhất
        const float towerX = toWorldCenterX(tower.pos.x);
        const float towerY = toWorldCenterY(tower.pos.y);
        Enemy* target = nullptr;
        float bestDistance = effectiveRange * effectiveRange;

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
            projectile.damage = effectiveDamage;
            
            // Đảm bảo đạn có tốc độ bay
            projectile.speed = (stats.projectileSpeed > 0) ? stats.projectileSpeed : 350.0f; 
            projectile.splashRadius = stats.splashRadius;
            projectile.sourceType = tower.type;
            projectile.isFrost = stats.isFrost;
            projectile.isElectric = (tower.type == TowerType::Electric);
            
            data.projectiles.push_back(projectile);
            tower.fireTimer = 0.0f;
        }
    }

    // === PROJECTILES ===
    for (auto& projectile : data.projectiles) {
        if (!projectile.active) continue;
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

        // KHI ĐẠN CHẠM MỤC TIÊU
        if (dist <= projectile.speed * deltaTime || dist <= 1.0f) {
            // 1. Kích hoạt hiệu ứng hình ảnh
            if (projectile.isFrost) {
                target->slowTimer = 2.0f;
                EffectsSystem::createSparkles(data, targetX, targetY, 6, 100, 150, 255);
            }
            if (projectile.sourceType == TowerType::Electric) {
                EffectsSystem::createSparkles(data, targetX, targetY, 8, 200, 200, 255);
            }
            if (projectile.sourceType == TowerType::Cannon) {
                EffectsSystem::createExplosion(data, targetX, targetY, 15, 255, 140, 0);
            }
            if (projectile.sourceType == TowerType::Tesla) {
                EffectsSystem::createSparkles(data, targetX, targetY, 12, 120, 255, 120);
                EffectsSystem::createExplosion(data, targetX, targetY, 8, 120, 255, 120);
            }
            
            // 2. Kích hoạt logic Sát thương riêng biệt cho từng tháp
            if (projectile.sourceType == TowerType::Cannon || projectile.sourceType == TowerType::Tesla) {
                // Sát thương AOE (Nổ lan)
                float radius = (projectile.splashRadius > 0) ? projectile.splashRadius : TILE_SIZE * 1.5f;
                auto aoeEnemies = getEnemiesInRange(data, targetX, targetY, radius);
                for (auto aoeTarget : aoeEnemies) {
                    aoeTarget->hp -= projectile.damage;
                    if (aoeTarget->hp <= 0) {
                        aoeTarget->active = false;
                        data.gold += aoeTarget->reward;
                        data.totalEnemiesKilled++;
                    }
                }
            } 
            else if (projectile.sourceType == TowerType::Electric) {
                // Sát thương điện - Giật mục tiêu chính
                target->hp -= projectile.damage;
                if (target->hp <= 0) {
                    target->active = false;
                    data.gold += target->reward;
                    data.totalEnemiesKilled++;
                }
                
                // Chain Lightning (Giật dây chuyền sang mục tiêu khác)
                std::vector<Enemy*> chainTargets;
                chainTargets.push_back(target);
                for (int chain = 0; chain < 2; ++chain) { 
                    Enemy* nextTarget = nullptr;
                    float nextDist = TILE_SIZE * 2.0f * TILE_SIZE * 2.0f;
                    Position lastPos = data.enemyPath[chainTargets.back()->currentStep];
                    float lastX = toWorldCenterX(lastPos.x);
                    float lastY = toWorldCenterY(lastPos.y);
                    
                    for (auto& enemy : data.enemies) {
                        if (!enemy.active) continue;
                        bool alreadyChained = false;
                        for (auto c : chainTargets) {
                            if (c->id == enemy.id) { alreadyChained = true; break; }
                        }
                        if (alreadyChained) continue;
                        
                        Position ePos = data.enemyPath[enemy.currentStep];
                        float eX = toWorldCenterX(ePos.x);
                        float eY = toWorldCenterY(ePos.y);
                        float d = distanceSquared(lastX, lastY, eX, eY);
                        if (d < nextDist) {
                            nextDist = d;
                            nextTarget = &enemy;
                        }
                    }
                    
                    if (nextTarget) {
                        Position ePos = data.enemyPath[nextTarget->currentStep];
                        EffectsSystem::createElectricSparkEffect(data, lastX, lastY, toWorldCenterX(ePos.x), toWorldCenterY(ePos.y));
                        nextTarget->hp -= projectile.damage / 2;
                        if (nextTarget->hp <= 0) {
                            nextTarget->active = false;
                            data.gold += nextTarget->reward;
                            data.totalEnemiesKilled++;
                        }
                        chainTargets.push_back(nextTarget);
                    } else {
                        break;
                    }
                }
            } 
            else {
                // Tháp Basic & Frost (Sát thương đơn mục tiêu)
                target->hp -= projectile.damage;
                if (target->hp <= 0) {
                    target->active = false;
                    data.gold += target->reward;
                    data.totalEnemiesKilled++;
                    EffectsSystem::createExplosion(data, targetX, targetY, 10, 200, 100, 50);
                }
            }
            
            projectile.active = false; // Hủy đạn sau khi trúng
        } else {
            // 3. Đạn tiếp tục bay nếu chưa tới đích
            projectile.x += (dx / dist) * projectile.speed * deltaTime;
            projectile.y += (dy / dist) * projectile.speed * deltaTime;
        }
    }

    // === UPDATE VISUAL EFFECTS ===
    EffectsSystem::updateParticles(data, deltaTime);
    EffectsSystem::updateEffects(data, deltaTime);

    // === CLEANUP ===
    data.enemies.erase(std::remove_if(data.enemies.begin(), data.enemies.end(), 
                                     [](const Enemy& e) { return !e.active; }), 
                      data.enemies.end());
    data.projectiles.erase(std::remove_if(data.projectiles.begin(), data.projectiles.end(), 
                                         [](const Projectile& p) { return !p.active; }), 
                          data.projectiles.end());
    data.operators.erase(std::remove_if(data.operators.begin(), data.operators.end(), 
                                       [](const Operator& op) { return !op.active; }), 
                        data.operators.end());
}