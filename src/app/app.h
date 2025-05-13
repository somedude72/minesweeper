#pragma once

#include <QApplication>

#include "view/game.h"
#include "model/data.h"
#include "model/board.h"
#include "model/screen.h"

class App : public QApplication {
public:
    App(int argc, char** argv);
    ~App();

private:
    void setupLCD();
    
    void gameOverRevealMines(const GameBoardCoord& cause);
    void gameWonMarkMines();
    void markNoQuestionImpl(const GameBoardCoord& coord);
    void markYesQuestionImpl(const GameBoardCoord& coord);

private slots:
    void onRestart();
    void onMark(const GameBoardCoord& coord);
    void onReveal(const GameBoardCoord& coord);
    void onOptionsChanged(const GameSettings& settings);

    // these functions implement the feature where when you click a number to reveal and
    // before you lift your mouse button, the surrounding 8 squares flash blank. these are
    // visual changes only
    void onRevealAltDown(const GameBoardCoord& coord);
    void onRevealAltUp(const GameBoardCoord& coord);

    void onTimerUpdated();

    void onActionAbout();
    void onActionBeginner();
    void onActionIntermediate();
    void onActionAdvanced();
    void onActionOptions() const;
    void onActionGithub() const;
    void onActionTutorial() const;

    void onClosePressed();
    void onMinimizePressed();

private:
    GameSettings m_settings;
    GameState m_state;
    GameBoard m_board;
    GameView* m_game_window = nullptr;
    QTimer* m_timer = nullptr;

    const int32_t min_size = minScreenSize();
};