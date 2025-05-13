#pragma once

#include <cstdint>
#include <QString>

struct GameState {
    bool won = false;
    bool lost = false;
    bool revealing_mine = false;
    bool is_first_reveal = true; 
    int mines = -1, timer = -1;
    bool operator==(const GameState& other) const = default;
    bool operator!=(const GameState& other) const = default;
};

struct GameSettings {
    int32_t row_size = 9;
    int32_t col_size = 9; 
    int32_t num_mines = 10;
    uint32_t seed = -1;
    bool is_question_enabled = false;
    bool is_safe_first_move = true;
    bool is_clear_first_move = false;
    bool is_set_seed = false;
};

struct GameStatistics {
    double best_time_all;
    double best_time_easy;
    double best_time_intermediate;
    double best_time_advanced;

    int32_t total_played;
    int32_t total_won;
    double total_ratio;

    int32_t easy_played;
    int32_t easy_won;
    double easy_ratio;

    int32_t intermediate_played;
    int32_t intermediate_won;
    double intermediate_ratio;

    int32_t advanced_played;
    int32_t advanced_won;
    double advanced_ratio;

    double play_time;
};

class GameDataManager {
public:
    GameDataManager();

    const GameSettings& getSettings() const;
    GameSettings& getSettings();
    void loadSettings();
    void saveSettings();

    const GameStatistics& getStatistics() const;
    GameStatistics& getStatistics();
    void loadStatistics();
    void saveStatistics();

private:
    QString m_config_path;
    QString m_stat_path;
    GameState m_state;
    GameStatistics m_statistics;
};