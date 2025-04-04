#pragma once

#include "QApplication"

#include "view/game.h"
#include "model/data.h"

class App : public QApplication {
public:
    App(int argc, char** argv);
    ~App();

private:
    void reveal_mines(const model::MineCoord& cause);

private slots:
    void on_restart();
    void on_mark(const model::MineCoord& coord);
    void on_reveal(const model::MineCoord& coord);

private:
    model::MineBoard m_board;
    view::MineWindow* m_window;
    bool m_game_over, m_game_won;

};