#pragma once

#include "QApplication"

#include "view/window.h"
#include "model/data.h"

class App : public QApplication {
public:
    App(int argc, char** argv);
    ~App();

private:
    void revealMines(const model::MineCoord& cause);

private slots:
    void onRestart();
    void onMark(const model::MineCoord& coord);
    void onReveal(const model::MineCoord& coord);

    void onClose();
    void onMinimize();

private:
    model::MineBoard m_board;
    view::MineWindow* m_window;
    bool m_game_over, m_game_won;

};