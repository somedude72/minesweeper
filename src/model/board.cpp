#include <ctime>
#include <cassert>
#include <cstdint>
#include <cstdlib>

#include <random>
#include <vector>
#include <queue>

#include "model/board.h"

namespace {

    // whether or not a coordinate is valid within a larger matrix/2d array
    bool isValid(int32_t curr_row, int32_t curr_col, int32_t max_row, int32_t max_col) {
        return curr_row >= 0 && curr_col >= 0 && curr_row < max_row && curr_col < max_col;
    }

    // generate a random number in the [lower, upper] interval inclusive
    int32_t randomNum(int32_t lower_range, int32_t upper_range, std::mt19937& seed) {
        assert(upper_range > lower_range);
        std::uniform_int_distribution<int32_t> dist(lower_range, upper_range);
        return dist(seed);
    }

}

GameBoard::GameBoard(const GameSettings& settings) : m_settings(settings) {
    m_board.assign(m_settings.row_size, std::vector<GameBoardSquare>(
        m_settings.col_size, 
        GameBoardSquare()
    ));
}

void GameBoard::generateMines(const GameBoardCoord& init) {
    generateMinesImpl(init);
    countAdjacent();
}

void GameBoard::floodfill(const GameBoardCoord& start) {
    std::queue<GameBoardCoord> queue;
    queue.push(start);

    while (!queue.empty()) {
        GameBoardCoord curr = queue.front();
        m_board[curr.row][curr.col].is_revealed = true;
        queue.pop();
        constexpr int dir_row[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
        constexpr int dir_col[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };

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

bool GameBoard::revealAdjacent(const GameBoardCoord& coord) {
    bool is_mine = false;
    int flag_nums = 0;

    constexpr int dir_row[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    constexpr int dir_col[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    for (int i = 0; i < 8; i++) {
        const int new_row = coord.row + dir_row[i];
        const int new_col = coord.col + dir_col[i];
        if (!isValid(new_row, new_col, rowSize(), colSize()))
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
        const int new_row = coord.row + dir_row[i];
        const int new_col = coord.col + dir_col[i];
        if (!isValid(new_row, new_col, rowSize(), colSize()) || m_board[new_row][new_col].is_marked)
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

namespace {

    // whether or a coordinate is a valid spot to generate a mine if the safe first click
    bool isValidSafeMine(int32_t row, int32_t col, int32_t reveal_row, int32_t reveal_col) {
        return row != reveal_row || col != reveal_col;
    }

    bool isValidClearMine(int32_t row, int32_t col, int32_t reveal_row, int32_t reveal_col) {
        return std::abs(reveal_row - row) > 1 || std::abs(reveal_col - col) > 1;
    }

}

void GameBoard::generateMinesImpl(const GameBoardCoord& guarantee) {
    std::random_device rd;
    std::mt19937 mt(rd());

    bool (*validCondition)(int32_t, int32_t, int32_t, int32_t);
    if (m_settings.is_safe_first_move)
        validCondition = isValidSafeMine;
    if (m_settings.is_clear_first_move)
        validCondition = isValidClearMine;

    for (int32_t i = 0; i < m_settings.num_mines; i++) {
        int32_t curr_row = randomNum(0, rowSize() - 1, mt);
        int32_t curr_col = randomNum(0, colSize() - 1, mt);
        while (m_board[curr_row][curr_col].is_mine || !validCondition(curr_row, curr_col, guarantee.row, guarantee.col)) {
            curr_row = randomNum(0, rowSize() - 1, mt);
            curr_col = randomNum(0, colSize() - 1, mt);
        }

        m_board[curr_row][curr_col].is_mine = true;
    }
}

void GameBoard::countAdjacent() {
    const int32_t max_row = rowSize();
    const int32_t max_col = colSize();
    for (int32_t i = 0; i < max_row; i++) {
        for (int32_t j = 0; j < max_col; j++) {
            constexpr int dir_row[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
            constexpr int dir_col[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
            
            for (int32_t k = 0; k < 8; k++) {
                const int adj_row = i + dir_row[k];
                const int adj_col = j + dir_col[k];
                if (adj_row < 0 || adj_col < 0 || adj_row >= max_row || adj_col >= max_col)
                    continue;
                if (m_board[adj_row][adj_col].is_mine) {
                    m_board[i][j].adjacent_mines++;
                }
            }
        }
    }
}

const GameBoardSquare& GameBoard::getSquare(const GameBoardCoord& get_coord) const {
    return m_board[get_coord.row][get_coord.col];
}

GameBoardSquare& GameBoard::getSquare(const GameBoardCoord& get_coord) {
    return m_board[get_coord.row][get_coord.col];
}

int32_t GameBoard::rowSize() const {
    return m_board.size();
}

int32_t GameBoard::colSize() const {
    return m_board[0].size();
}

bool GameBoard::didWin() const {
    for (int i = 0; i < m_board.size(); i++) {
        for (int j = 0; j < m_board[0].size(); j++) {
            if (!m_board[i][j].is_revealed && !m_board[i][j].is_mine) {
                return false;
            } else if (m_board[i][j].is_revealed && m_board[i][j].is_mine) {
                return false;
            }
        }
    }

    return true;
}