#pragma once
#include <string>
#include <vector>

struct GameStats {
    int score;
    int wave;
    int enemiesKilled;
    int goldEarned;
    std::string date;
};

class ScoreManager {
private:
    static const char* SCORES_FILE;
    std::vector<GameStats> scores;
    
public:
    ScoreManager();
    
    // Tải high scores từ file
    void loadScores();
    
    // Lưu high scores vào file
    void saveScores();
    
    // Thêm điểm số mới
    void addScore(const GameStats& stats);
    
    // Lấy top 10 scores
    const std::vector<GameStats>& getTopScores() const;
    
    // Lấy high score
    int getHighScore() const;
};
