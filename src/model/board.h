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
    bool is_question = false;
    bool is_end_reason = false;
    bool operator==(const GameBoardSquare& other) const = default;
    bool operator!=(const GameBoardSquare& other) const = default;
};

class GameBoard {
public:
    GameBoard() = default;
    GameBoard(const GameBoard& other) = default;
    explicit GameBoard(const GameSettings& settings);
    
    void reset(const GameSettings& settings);
    void mark(const GameBoardCoord& coord, GameState& state);
    void reveal(const GameBoardCoord& coord, GameState& state);
    void revealAdjacentDown(const GameBoardCoord& coord);    
    void revealAdjacentUp();
    
    // seed of -1 (wraps to UINT32_MAX) means a random seed
    void generateMines(const GameBoardCoord& init);
    void updateSettings(const GameSettings& new_settings);

    int32_t rowSize() const;
    int32_t colSize() const;

    const GameBoardSquare& getSquare(const GameBoardCoord& get_coord) const;
    GameBoardSquare& getSquare(const GameBoardCoord& get_coord);
    uint32_t getSeed() const;
    
private:
    // this will reveal all neighboring squares that does not have mines adjacent to
    // them, starting from the start coordinate. note that this function will not
    // check if the row/col to start revealing at is a mine.
    void floodfillImpl(const GameBoardCoord& start);
    void generateMinesImpl(const GameBoardCoord& guarantee); 
    bool revealAdjacentImpl(const GameBoardCoord& coord);
    void countAdjacent(); 

    bool didWin() const;
    void gameOverRevealMines(const GameBoardCoord& last_reveal);
    void gameWonMarkMines();
    
private:
    GameSettings m_settings = GameSettings();
    std::vector<std::vector<GameBoardSquare>> m_internal_copy = {}; // for revealAdjacent visual changes
    std::vector<std::vector<GameBoardSquare>> m_board = {};
};