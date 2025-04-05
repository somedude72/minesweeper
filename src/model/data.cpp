#include "model/data.h"
#include "utils/config.h"
#include "utils/config.h"

#include <ctime>
#include <cassert>
#include <cstdint>
#include <cstdlib>

#include <vector>
#include <queue>

namespace {

int32_t random_num(int32_t lower_range, int32_t upper_range) {
    assert(upper_range > lower_range);
    int32_t diff = upper_range - lower_range;
    int32_t divisor = RAND_MAX / diff;
    return lower_range + (std::rand() / divisor);
}

} // internal namespace

namespace model {

MineBoard::MineBoard(uint32_t row_num, uint32_t col_num) {
    LOG_INFO("board: initializing {} by {} board", row_num, col_num);
    m_board.resize(row_num, std::vector<MineSquare>(col_num));
    generate_mines();
    count_adjacent();
}

void MineBoard::floodfill(const MineCoord& start) {
    std::queue<MineCoord> queue;

    queue.push(start);
    while (!queue.empty()) {
        MineCoord curr = queue.front();
        m_board[curr.row][curr.col].is_revealed = true;
        queue.pop();
        const int dir_row[4] = { 0, 0, -1, 1 };
        const int dir_col[4] = { -1, 1, 0, 0 }; 

        for (int32_t i = 0; i < 4; i++) {
            int32_t new_row = curr.row + dir_row[i];
            int32_t new_col = curr.col + dir_col[i];
            if (new_row < 0 || new_col < 0 || new_row >= row_size() || new_col >= col_size())
                continue;
            if (m_board[new_row][new_col].is_mine || m_board[new_row][new_col].is_revealed)
                continue;
            if (m_board[new_row][new_col].adjacent_mines) {
                m_board[new_row][new_col].is_revealed = true;
            } else {
                m_board[new_row][new_col].is_revealed = true;
                queue.push({ new_row, new_col });
            }
        }
    }
}

const MineSquare& MineBoard::get_square(const MineCoord& get_coord) const {
    return m_board[get_coord.row][get_coord.col];
}

MineSquare& MineBoard::get_square(const MineCoord& get_coord) {
    return m_board[get_coord.row][get_coord.col];
}

int32_t MineBoard::row_size() const {
    return m_board.size();
}

int32_t MineBoard::col_size() const {
    return m_board[0].size();
}

bool MineBoard::did_win() const {
    for (int i = 0; i < m_board.size(); i++) {
        for (int j = 0; j < m_board[0].size(); j++) {
            if (!m_board[i][j].is_revealed && !m_board[i][j].is_mine) {
                return false;
            } else if (m_board[i][j].is_revealed && m_board[i][j].is_mine) {
                return false;
            }
        }
    }

    LOG_DEBUG("board: checking did_win = true");
    return true;
}

void MineBoard::generate_mines() {
    std::srand(std::time(nullptr));
    int32_t max_row = row_size();
    int32_t max_col = col_size();
    
    for (int32_t i = 0; i < max_row; i++) {
        for (int32_t j = 0; j < max_col; j++) {
            if (random_num(1, 10) == 1) {
                m_board[i][j].is_mine = true;
                m_board[i][j].adjacent_mines = 0;
            }
        }
    }
}

void MineBoard::count_adjacent() {
    int32_t max_row = row_size();
    int32_t max_col = col_size();

    for (int32_t i = 0; i < max_row; i++) {
        for (int32_t j = 0; j < max_col; j++) {
            const int dir_row[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
            const int dir_col[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
            
            for (int32_t k = 0; k < 8; k++) {
                int adj_row = i + dir_row[k];
                int adj_col = j + dir_col[k];
                if (adj_row < 0 || adj_col < 0 || adj_row >= max_row || adj_col >= max_col)
                    continue;
                if (m_board[adj_row][adj_col].is_mine) {
                    m_board[i][j].adjacent_mines++;
                }
            }
        }
    }
}

} // namespace model