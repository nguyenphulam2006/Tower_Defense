#include "ScoreManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

const char* ScoreManager::SCORES_FILE = "scores.txt";

ScoreManager::ScoreManager() {
    loadScores();
}

void ScoreManager::loadScores() {
    scores.clear();
    std::ifstream file(SCORES_FILE);
    if (!file.is_open()) {
        return; // File not found, start with empty scores
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        // Parse format: score wave enemies gold date
        int score = 0, wave = 0, enemies = 0, gold = 0;
        std::string date;
        
        std::istringstream stream(line);
        if (stream >> score >> wave >> enemies >> gold) {
            std::getline(stream, date);
            if (!date.empty() && date[0] == ' ') {
                date = date.substr(1);
            }
            scores.push_back({score, wave, enemies, gold, date});
        }
    }
    file.close();

    // Sort by score descending
    std::sort(scores.begin(), scores.end(), 
              [](const GameStats& a, const GameStats& b) { return a.score > b.score; });

    // Keep only top 20
    if (scores.size() > 20) {
        scores.resize(20);
    }
}

void ScoreManager::saveScores() {
    std::ofstream file(SCORES_FILE);
    if (!file.is_open()) return;

    file << "# High Scores - Score Wave Enemies Gold Date\n";
    for (const auto& stats : scores) {
        file << stats.score << " " << stats.wave << " " << stats.enemiesKilled 
             << " " << stats.goldEarned << " " << stats.date << "\n";
    }
    file.close();
}

void ScoreManager::addScore(const GameStats& stats) {
    // Get current date/time
    time_t now = time(0);
    struct tm* timeinfo = localtime(&now);
    char buffer[32];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", timeinfo);
    
    GameStats newStats = stats;
    newStats.date = std::string(buffer);
    newStats.score = stats.wave * 100 + stats.enemiesKilled * 5 + stats.goldEarned;
    
    scores.push_back(newStats);
    
    // Sort by score
    std::sort(scores.begin(), scores.end(),
              [](const GameStats& a, const GameStats& b) { return a.score > b.score; });
    
    // Keep only top 20
    if (scores.size() > 20) {
        scores.resize(20);
    }
    
    saveScores();
}

const std::vector<GameStats>& ScoreManager::getTopScores() const {
    return scores;
}

int ScoreManager::getHighScore() const {
    if (scores.empty()) return 0;
    return scores[0].score;
}
