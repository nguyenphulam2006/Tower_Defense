#pragma once
#include "GameData.h"
#include <string>

class MapSystem {
public:
    // Khởi tạo trạng thái đầu game (load map 1 nhưng ở Menu)
    static void initGame(GameData& data);
    
    // Reset toàn bộ thông số và load map khi ấn Play hoặc Retry
    static void resetGame(GameData& data, int levelToLoad = -1);

private:
    static void loadMapFromFile(GameData& data, const std::string& filepath);
    static void loadAndGeneratePath(GameData& data);
};