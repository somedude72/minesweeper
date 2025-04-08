#pragma once

#include <vector>
#include <cstdint>

namespace model {

struct GameSettings;    // Represents the user defined settings of the game
struct GameState;       // Represents whether or not the user has won, lost,
                        // or is currently revealing a mine
struct MineSquare;      // Data representation of a square in MineBoard
struct MineCoord;       // For succinct representation of a coordinate when
                        // interfacing with the MineBoard class. 
class MineBoard {
public:

    MineBoard() = default;
    MineBoard(const MineBoard& other) = default;

    // Construct a new MineBoard object with row number of rows and col number of
    // columns; also procedurally generating the mines.
    MineBoard(uint32_t row, uint32_t col);

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
    std::vector<std::vector<MineSquare>> m_board;

};

struct GameSettings {
    uint32_t row_size, col_size, num_mines;
    bool is_safe_first_move;
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

} // namespace model