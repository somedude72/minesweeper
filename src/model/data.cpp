#include "model/data.h"
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
    generateMines();
    countAdjacent();
}

void MineBoard::floodfill(const MineCoord& start) {
    std::queue<MineCoord> queue;

    queue.push(start);
    while (!queue.empty()) {
        MineCoord curr = queue.front();
        m_board[curr.row][curr.col].is_revealed = true;
        queue.pop();
        const int dir_row[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
        const int dir_col[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

        if (m_board[curr.row][curr.col].adjacent_mines)
            continue;
        for (int32_t i = 0; i < 8; i++) {
            int32_t new_row = curr.row + dir_row[i];
            int32_t new_col = curr.col + dir_col[i];
            if (new_row < 0 || new_col < 0 || new_row >= rowSize() || new_col >= colSize())
                continue;
            if (m_board[new_row][new_col].is_mine || m_board[new_row][new_col].is_revealed)
                continue;
            if (m_board[new_row][new_col].is_marked)
                continue;
            m_board[new_row][new_col].is_revealed = true;
            queue.push({ new_row, new_col });
        }
    }
}

bool MineBoard::revealAdjacent(const MineCoord& coord) {
    int flag_nums = 0;
    bool is_mine = false;
    const int dir_row[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    const int dir_col[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    for (int i = 0; i < 8; i++) {
        int new_row = coord.row + dir_row[i];
        int new_col = coord.col + dir_col[i];
        if (new_row < 0 || new_col < 0 || new_row >= rowSize() || new_col >= colSize())
            continue;
        if (m_board[new_row][new_col].is_marked) {
            flag_nums++;
        } else if (m_board[new_row][new_col].is_mine) {
            is_mine = true;
        }
    }


    if (flag_nums != m_board[coord.row][coord.col].adjacent_mines)
        return false;
    for (int i = 0; i < 8; i++) {
        int new_row = coord.row + dir_row[i];
        int new_col = coord.col + dir_col[i];
        if (new_row < 0 || new_col < 0 || new_row >= rowSize() || new_col >= colSize() || m_board[new_row][new_col].is_marked)
            continue;
        if (m_board[new_row][new_col].is_mine) {
            m_board[new_row][new_col].is_end_reason = true;
        } else if (is_mine) {
            m_board[new_row][new_col].is_revealed = true;
        } else {
            floodfill({ new_row, new_col });
        }
    }
    
    return is_mine;
}

const MineSquare& MineBoard::getSquare(const MineCoord& get_coord) const {
    return m_board[get_coord.row][get_coord.col];
}

MineSquare& MineBoard::getSquare(const MineCoord& get_coord) {
    return m_board[get_coord.row][get_coord.col];
}

int32_t MineBoard::rowSize() const {
    return m_board.size();
}

int32_t MineBoard::colSize() const {
    return m_board[0].size();
}

bool MineBoard::didWin() const {
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

void MineBoard::generateMines() {
    std::srand(std::time(nullptr));
    int32_t max_row = rowSize();
    int32_t max_col = colSize();
    
    for (int32_t i = 0; i < max_row; i++) {
        for (int32_t j = 0; j < max_col; j++) {
            if (random_num(1, 8) == 1) {
                m_board[i][j].is_mine = true;
                m_board[i][j].adjacent_mines = 0;
            }
        }
    }
}

void MineBoard::countAdjacent() {
    int32_t max_row = rowSize();
    int32_t max_col = colSize();

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