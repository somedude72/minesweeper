#pragma once

#include <vector>
#include <cstdint>

#include "model/data.h"

struct GameBoardCoord {
    int32_t row, col;
};

struct GameBoardSquare {
    int adjacent_mines = 0;
    bool is_mine = false;
    bool is_revealed = false;
    bool is_marked = false;
    bool is_end_reason = false;
    bool operator==(const GameBoardSquare& other) const = default;
    bool operator!=(const GameBoardSquare& other) const = default;
};

class GameBoard {
public:
    GameBoard() = default;
    GameBoard(const GameBoard& other) = default;
    explicit GameBoard(const GameSettings& settings);
    
    void generateMines(const GameBoardCoord& init);
    // this will reveal all neighboring squares that does not have mines adjacent to
    // them, starting from the start coordinate. note that this function will not
    // check if the row/col to start revealing at is a mine.
    void floodfill(const GameBoardCoord& start);
    // this implements the mechanism where when the player clicks a revealed square that
    // has a number of adjacent mines, and all mines have been marked, the game auto
    // reveals the rest of the adjacent squares. returns whether or not the revealed
    // squares contains any mines. 
    bool revealAdjacent(const GameBoardCoord& coord);
    
    bool didWin() const;
    int32_t rowSize() const;
    int32_t colSize() const;
    const GameBoardSquare& getSquare(const GameBoardCoord& get_coord) const;
    GameBoardSquare& getSquare(const GameBoardCoord& get_coord);

private:
    void generateMinesImpl(const GameBoardCoord& guarantee); 
    void countAdjacent(); 

private:
    GameSettings m_settings = GameSettings();
    std::vector<std::vector<GameBoardSquare>> m_board = {};
};