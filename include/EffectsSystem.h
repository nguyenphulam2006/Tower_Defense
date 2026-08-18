#pragma once
#include "GameData.h"
#include <cmath>

class EffectsSystem {
public:
    // Tạo hiệu ứng nổ
    static void createExplosion(GameData& data, float x, float y, int particleCount = 12, 
                               int r = 255, int g = 180, int b = 0);
    
    // Tạo hiệu ứng tia điện
    static void createElectricSparkEffect(GameData& data, float x1, float y1, float x2, float y2);
    
    // Tạo hiệu ứng xích tụ điện
    static void createChainLightning(GameData& data, const std::vector<Position>& chainPath);
    
    // Tạo hiệu ứng đốm sáng (healing/buff)
    static void createSparkles(GameData& data, float x, float y, int count = 8,
                              int r = 0, int g = 255, int b = 150);
    
    // Update tất cả particles
    static void updateParticles(GameData& data, float deltaTime);
    
    // Update tất cả visual effects
    static void updateEffects(GameData& data, float deltaTime);

private:
    static float randomFloat(float min, float max);
};
