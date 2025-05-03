#pragma once

#include <cstdint>

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
    bool is_safe_first_move = true;
    bool is_clear_first_move = false;
};