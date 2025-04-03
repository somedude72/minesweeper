#include "QApplication"
#include "spdlog/spdlog.h"

#include "app/app.h"
#include "view/game.h"
#include "model/data.h"

App::App(int argc, char** argv) : QApplication(argc, argv) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%a, %b %d %H:%M:%S] [%l] %v");
    spdlog::info("logging initiated (this is a test message)");

    m_board = model::MineBoard(10, 15);
    m_window = new view::MineWindow(m_board);
    connect(m_window, &view::MineWindow::restart, this, &App::on_restart);
    connect(m_window, &view::MineWindow::reveal, this, &App::on_reveal);
    connect(m_window, &view::MineWindow::mark, this, &App::on_mark);
    
    m_window->show();
}

App::~App() {
    delete m_window;
}

void App::game_over() {
    for (int32_t i = 0; i < m_board.row_size(); i++)
        for (int32_t j = 0; j < m_board.col_size(); j++)
            m_board.get_square({ i, j }).is_revealed = true;
    m_window->update_board(m_board);
}

void App::on_restart() {
    spdlog::debug("restarting game\n");
    m_board = model::MineBoard(10, 15);
    m_window->update_board(m_board);
}

void App::on_mark(const model::MineCoord& coord) {
    spdlog::debug("marking ({}, {})", coord.row, coord.col);
    model::MineSquare& curr = m_board.get_square(coord);
    curr.is_marked = !curr.is_marked;
    m_window->update_board(m_board);
}

void App::on_reveal(const model::MineCoord& coord) {
    spdlog::debug("revealing ({}, {})", coord.row, coord.col);
    model::MineSquare& curr = m_board.get_square(coord);
    if (curr.is_mine) {
        game_over();
    } else if (curr.adjacent_mines) {
        m_board.get_square(coord).is_revealed = true;
        m_window->update_board(m_board);
    } else {
        m_board.floodfill(coord);
        m_window->update_board(m_board);
    }
}