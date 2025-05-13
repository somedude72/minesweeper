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
    m_settings = settings;
    m_board.assign(m_settings.row_size, std::vector<GameBoardSquare>(
        m_settings.col_size, 
        GameBoardSquare()
    ));
}

void GameBoard::generateMines(const GameBoardCoord& init) {
    generateMinesImpl(init);
    countAdjacent();
}

void GameBoard::updateSettings(const GameSettings& new_settings) {
    m_settings = new_settings;
}

void GameBoard::gameOverRevealMines(const GameBoardCoord& last_reveal) {
    for (int32_t i = 0; i < rowSize(); i++) {
        for (int32_t j = 0; j < colSize(); j++) {
            if (!m_board[i][j].is_mine && !m_board[i][j].is_marked)
                continue; // we do not want to do anything with marked mines
            if (i == last_reveal.row && j == last_reveal.col) {
                m_board[i][j].is_revealed = true;
                m_board[i][j].is_end_reason = true;
            } else if (!m_board[i][j].is_mine && m_board[i][j].is_marked) {
                m_board[i][j].is_revealed = true; // wrongly marked mine
            } else if (m_board[i][j].is_mine && !m_board[i][j].is_marked) {
                m_board[i][j].is_revealed = true; // not marked mine
            }
        }
    }
}

void GameBoard::gameWonMarkMines() {
    for (int32_t i = 0; i < m_board.size(); i++) {
        for (int32_t j = 0; j < m_board[0].size(); j++) {
            if (m_board[i][j].is_mine) {
                m_board[i][j].is_marked = true;
            }
        }
    }
}

void GameBoard::mark(const GameBoardCoord& coord, GameState& state) {
    GameBoardSquare& square = m_board[coord.row][coord.col];
    if (state.lost || state.won || square.is_revealed)
        return;
    if (m_settings.is_question_enabled) {
        if (square.is_marked) {
            square.is_marked = false;
            square.is_question = true;
            state.mines++;
        } else if (square.is_question) {
            square.is_question = false;
        } else {
            square.is_marked = true;
            state.mines--;
        }
    } else {
        if (square.is_marked) {
            square.is_marked = false;
            state.mines++;
        } else {
            square.is_marked = true;
            state.mines--;
        }
    }
}

void GameBoard::reveal(const GameBoardCoord& coord, GameState& state) {
    if (state.lost || state.won || m_board[coord.row][coord.col].is_marked)
        return;
    if (m_board[coord.row][coord.col].is_mine) {
        state.lost = true;
        gameOverRevealMines(coord);
    } else if (!m_board[coord.row][coord.col].is_revealed) {
        floodfillImpl(coord);
    } else if (revealAdjacentImpl(coord)) {
        gameOverRevealMines(coord);
        state.lost = true;
    }

    if (didWin()) {
        gameWonMarkMines();
        state.won = true;
    }

    state.is_first_reveal = false;
}

void GameBoard::reset(const GameSettings& settings) {
    m_settings = settings;
    m_board.assign(m_settings.row_size, std::vector<GameBoardSquare>(
        m_settings.col_size, 
        GameBoardSquare()
    ));
}

void GameBoard::revealAdjacentUp() {
    if (!m_internal_copy.empty()) {
        m_board = m_internal_copy;
        m_internal_copy = {};
    }
}

void GameBoard::revealAdjacentDown(const GameBoardCoord& coord) {
    const GameBoardSquare& square = m_board[coord.row][coord.col];
    if (!square.is_revealed || !square.adjacent_mines)
        return;

    m_internal_copy = m_board;
    constexpr int dir_row[8] = { -1, -1, -1, 0, 0, 1, 1, 1 };
    constexpr int dir_col[8] = { -1, 0, 1, -1, 1, -1, 0, 1 };
    for (int32_t i = 0; i < 8; i++) {
        const int32_t new_row = coord.row + dir_row[i];
        const int32_t new_col = coord.col + dir_col[i];
        GameBoardSquare& sq = m_board[new_row][new_col];
        if (!isValid(new_row, new_col, rowSize(), colSize()))
            continue;
        if (!sq.is_marked && !sq.is_revealed) {
            sq.is_revealed = true;
            sq.is_mine = false;
            sq.adjacent_mines = 0;
        }
    }
}

const GameBoardSquare& GameBoard::getSquare(const GameBoardCoord& get_coord) const {
    return m_board[get_coord.row][get_coord.col];
}

GameBoardSquare& GameBoard::getSquare(const GameBoardCoord& get_coord) {
    return m_board[get_coord.row][get_coord.col];
}

uint32_t GameBoard::getSeed() const {
    return m_settings.seed;
}

int32_t GameBoard::rowSize() const {
    return m_board.size();
}

int32_t GameBoard::colSize() const {
    return m_board[0].size();
}

namespace {

    // whether or a coordinate is a valid spot to generate a mine if the safe first click
    bool isOutsideSafeZone(int32_t row, int32_t col, int32_t reveal_row, int32_t reveal_col) {
        return row != reveal_row || col != reveal_col;
    }

    bool isOutsideClearZone(int32_t row, int32_t col, int32_t reveal_row, int32_t reveal_col) {
        return std::abs(reveal_row - row) > 1 || std::abs(reveal_col - col) > 1;
    }

    bool alwaysValid(int32_t, int32_t, int32_t, int32_t) {
        return true;
    }

}

void GameBoard::generateMinesImpl(const GameBoardCoord& guarantee) {
    std::mt19937 engine(m_settings.seed);

    bool (*validCondition)(int32_t, int32_t, int32_t, int32_t) = alwaysValid;
    if (m_settings.is_safe_first_move)
        validCondition = isOutsideSafeZone;
    if (m_settings.is_clear_first_move)
        validCondition = isOutsideClearZone;

    for (int32_t i = 0; i < m_settings.num_mines; i++) {
        int32_t curr_row = randomNum(0, rowSize() - 1, engine);
        int32_t curr_col = randomNum(0, colSize() - 1, engine);
        while (m_board[curr_row][curr_col].is_mine || !validCondition(curr_row, curr_col, guarantee.row, guarantee.col)) {
            curr_row = randomNum(0, rowSize() - 1, engine);
            curr_col = randomNum(0, colSize() - 1, engine);
        }

        m_board[curr_row][curr_col].is_mine = true;
    }
}

bool GameBoard::revealAdjacentImpl(const GameBoardCoord& coord) {
    revealAdjacentUp();
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
            floodfillImpl({ new_row, new_col });
        }
    }
    
    return is_mine;
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

void GameBoard::floodfillImpl(const GameBoardCoord& start) {
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
