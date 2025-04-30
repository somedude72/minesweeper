#pragma once

#include <vector>
#include <cstdint>

namespace model {

struct GameSettings {
    uint32_t row_size = 9, col_size = 9, num_mines = 10;
    bool is_safe_first_move = true;
};

struct GameState {
    bool win, lose, revealing_mine;

    inline bool operator==(const GameState& other) const {
        return this->win == other.win &&
            this->lose == other.lose &&
            this->revealing_mine == other.revealing_mine;
    }

    inline bool operator!=(const GameState& other) const {
        return !(*this == other);
    }
};

struct MineCoord {
    int32_t row, col;
};

struct MineSquare {
    int adjacent_mines = 0;
    bool is_mine = false, is_marked = false;
    bool is_revealed = false, is_end_reason = false;

    inline bool operator==(const MineSquare& other) const {
        return this->adjacent_mines == other.adjacent_mines &&
            this->is_mine == other.is_mine &&
            this->is_marked == other.is_marked &&
            this->is_revealed == other.is_revealed &&
            this->is_end_reason == other.is_end_reason;
    }

    inline bool operator!=(const MineSquare& other) const {
        return !(*this == other);
    }
};


class MineBoard {
public:

    MineBoard() = default;
    MineBoard(const MineBoard& other) = default;

    // Construct a new MineBoard object with row number of rows and col number of
    // columns; also procedurally generating the mines.
    MineBoard(const GameSettings& settings);
    void setupBoard();

    // This will reveal all neighboring squares that does not have mines adjacent to
    // them, starting from the start coordinate. Note that this function will not
    // check if the row/col to start revealing at is a mine.
    void floodfill(const MineCoord& start);

    const MineSquare& getSquare(const MineCoord& get_coord) const;
    MineSquare& getSquare(const MineCoord& get_coord);

    bool revealAdjacent(const MineCoord& coord);
    bool didWin() const;
    
    int32_t rowSize() const;
    int32_t colSize() const;

private:
    void generateMines(); 
    void countAdjacent(); 

private:
    GameSettings m_settings;
    std::vector<std::vector<MineSquare>> m_board;

};

} // namespace model