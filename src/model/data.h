#pragma once

#include <vector>

namespace model {

    struct MineCoord {
        int32_t row, col;
        bool operator==(const MineCoord& other) const;
        bool operator!=(const MineCoord& other) const;
    };

    struct MineSquare {
        int adjacent_mines;
        bool is_mine, is_revealed;
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
        void set_square(const MineCoord& set_coord, const MineSquare& value);

        int32_t row_size() const;
        int32_t col_size() const;

    private:
        void generate_mines(); 
        void count_adjacent(); 

    private:
        std::vector<std::vector<MineSquare>> m_board;

    };

} // namespace model