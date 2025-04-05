#pragma once

#include <vector>

namespace model {

struct MineCoord {
    int32_t row, col;
};

struct MineSquare {
    int adjacent_mines = 0;
    bool is_mine = false, is_marked = false;
    bool is_revealed = false, is_end_reason = false;
};

class MineBoard {
public:

    MineBoard() = default;

    // Construct a new MineBoard object with row number of rows and col number of
    // columns; also procedurally generating the mines.
    MineBoard(uint32_t row, uint32_t col);

    // This will reveal all neighboring squares that does not have mines adjacent to
    // them, starting from the start coordinate. Note that this function will not
    // check if the row/col to start revealing at is a mine.
    void floodfill(const MineCoord& start);

    const MineSquare& get_square(const MineCoord& get_coord) const;
    MineSquare& get_square(const MineCoord& get_coord);

    int32_t row_size() const;
    int32_t col_size() const;

    bool did_win() const;

private:
    void generate_mines(); 
    void count_adjacent(); 

private:
    std::vector<std::vector<MineSquare>> m_board;

};

} // namespace model