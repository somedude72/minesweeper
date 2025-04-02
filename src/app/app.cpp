#include <QAPplication>
#include <iostream>

#include "app/app.h"
#include "view/game.h"
#include "model/data.h"

App::App(int argc, char** argv) : QApplication(argc, argv) {
    m_board = model::MineBoard(10, 15);
    m_window = new view::MineWindow(m_board);
    connect(m_window, &view::MineWindow::restart, this, &App::on_restart);
    connect(m_window, &view::MineWindow::reveal, this, &App::on_reveal);
    m_window->show();
}

void App::on_restart() {
    std::cout << "[app] restarting\n";
    m_board = model::MineBoard(10, 15);
    m_window->update(m_board);
}

void App::on_reveal(const model::MineCoord& coord) {
    std::cout << "[app] revealing " << coord.row << coord.col << '\n';
    model::MineSquare curr = m_board.get_square(coord);
    if (curr.is_mine) {
        std::cout << "Game over" << std::endl;
        on_restart();
        return;
    } else {
        m_board.floodfill(coord);
        m_window->update(m_board);
    }
}