#include "EffectsSystem.h"
#include <random>

static std::random_device rd;
static std::mt19937 gen(rd());

float EffectsSystem::randomFloat(float min, float max) {
    std::uniform_real_distribution<float> dis(min, max);
    return dis(gen);
}

void EffectsSystem::createExplosion(GameData& data, float x, float y, int particleCount, 
                                   int r, int g, int b) {
    for (int i = 0; i < particleCount; ++i) {
        Particle p;
        p.x = x;
        p.y = y;
        
        // Tia xuất phát từ tâm
        float angle = (2.0f * 3.14159f * i) / particleCount;
        float speed = randomFloat(100.0f, 300.0f);
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;
        
        p.lifetime = 0.0f;
        p.maxLifetime = randomFloat(0.3f, 0.7f);
        p.r = r;
        p.g = g;
        p.b = b;
        p.scale = randomFloat(0.5f, 1.5f);
        p.active = true;
        
        data.particles.push_back(p);
    }
}

void EffectsSystem::createSparkles(GameData& data, float x, float y, int count,
                                  int r, int g, int b) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.x = x;
        p.y = y;
        
        float angle = (2.0f * 3.14159f * i) / count;
        float speed = randomFloat(80.0f, 200.0f);
        p.vx = std::cos(angle) * speed;
        p.vy = std::sin(angle) * speed;
        
        p.lifetime = 0.0f;
        p.maxLifetime = randomFloat(0.4f, 0.8f);
        p.r = r;
        p.g = g;
        p.b = b;
        p.scale = randomFloat(0.3f, 1.0f);
        p.active = true;
        
        data.particles.push_back(p);
    }
}

void EffectsSystem::createElectricSparkEffect(GameData& data, float x1, float y1, float x2, float y2) {
    // Tạo vài spark particles dọc theo đường
    int sparkCount = 8;
    for (int i = 0; i < sparkCount; ++i) {
        float t = (float)i / sparkCount;
        Particle p;
        p.x = x1 + (x2 - x1) * t + randomFloat(-10.0f, 10.0f);
        p.y = y1 + (y2 - y1) * t + randomFloat(-10.0f, 10.0f);
        
        p.vx = randomFloat(-50.0f, 50.0f);
        p.vy = randomFloat(-50.0f, 50.0f);
        
        p.lifetime = 0.0f;
        p.maxLifetime = randomFloat(0.2f, 0.4f);
        p.r = 200;
        p.g = 200;
        p.b = 255;
        p.scale = randomFloat(0.2f, 0.6f);
        p.active = true;
        
        data.particles.push_back(p);
    }
}

void EffectsSystem::createChainLightning(GameData& data, const std::vector<Position>& chainPath) {
    for (size_t i = 0; i < chainPath.size() - 1; ++i) {
        float x1 = chainPath[i].x * 64.0f + 32.0f;
        float y1 = chainPath[i].y * 64.0f + 32.0f;
        float x2 = chainPath[i + 1].x * 64.0f + 32.0f;
        float y2 = chainPath[i + 1].y * 64.0f + 32.0f;
        createElectricSparkEffect(data, x1, y1, x2, y2);
    }
}

void EffectsSystem::updateParticles(GameData& data, float deltaTime) {
    for (auto& p : data.particles) {
        if (!p.active) continue;
        
        p.lifetime += deltaTime;
        if (p.lifetime >= p.maxLifetime) {
            p.active = false;
            continue;
        }
        
        // Chuyển động
        p.x += p.vx * deltaTime;
        p.y += p.vy * deltaTime;
        
        // Trọng lực (rơi nhẹ)
        p.vy += 100.0f * deltaTime;
        
        // Tính alpha dựa trên lifetime
        float alpha = 1.0f - (p.lifetime / p.maxLifetime);
        // alpha sẽ dùng trong vẽ (trong RenderSystem)
    }
    
    // Dọn dẹp particles đã chết
    data.particles.erase(
        std::remove_if(data.particles.begin(), data.particles.end(),
                      [](const Particle& p) { return !p.active; }),
        data.particles.end()
    );
}

void EffectsSystem::updateEffects(GameData& data, float deltaTime) {
    for (auto& e : data.effects) {
        if (!e.active) continue;
        
        e.lifetime += deltaTime;
        if (e.lifetime >= e.maxLifetime) {
            e.active = false;
            continue;
        }
    }
    
    // Dọn dẹp effects đã chết
    data.effects.erase(
        std::remove_if(data.effects.begin(), data.effects.end(),
                      [](const VisualEffect& e) { return !e.active; }),
        data.effects.end()
    );
}
