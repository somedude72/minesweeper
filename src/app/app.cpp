#include "QApplication"
#include "spdlog/spdlog.h"

#include "app/app.h"
#include "view/game.h"
#include "model/data.h"

App::App(int argc, char** argv) : QApplication(argc, argv) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%a, %b %d %H:%M:%S] [%l] %v");
    spdlog::info("app: logging initiated (this is a test message)");
    
    m_board = model::MineBoard(10, 15);
    m_window = new view::MineWindow(m_board);

    connect(m_window, &view::MineWindow::restart, this, &App::on_restart);
    connect(m_window, &view::MineWindow::reveal, this, &App::on_reveal);
    connect(m_window, &view::MineWindow::mark, this, &App::on_mark);

    m_window->show();
}

App::~App() {
    delete m_window;
    spdlog::info("app: deallocated window object");
    spdlog::info("app: terminating application");
}

void App::game_over(const model::MineCoord& cause) {
    spdlog::info("app: game over due to revealing mine");
    for (int32_t i = 0; i < m_board.row_size(); i++) {
        for (int32_t j = 0; j < m_board.col_size(); j++) {
            if (!m_board.get_square({ i, j }).is_mine)
                continue;
            if (i != cause.row || j != cause.col) {
                m_board.get_square({ i, j }).is_revealed = true;
            } else {
                m_board.get_square({ i, j }).is_revealed = true;
                m_board.get_square({ i, j }).is_reason_for_end = true;
            }
        }
    }

    m_window->update_board(m_board);
}

void App::on_restart() {
    spdlog::info("app: received restart game signal");
    m_board = model::MineBoard(10, 15);
    m_window->update_board(m_board);
}

void App::on_mark(const model::MineCoord& coord) {
    spdlog::info("app: received mark signal at ({}, {})", coord.row, coord.col);
    model::MineSquare& square = m_board.get_square(coord);
    
    square.is_marked = !square.is_marked;
    m_window->update_board(m_board);
}

void App::on_reveal(const model::MineCoord& coord) {
    spdlog::info("app: received reveal signal at ({}, {})", coord.row, coord.col);
    model::MineSquare& square = m_board.get_square(coord);

    if (square.is_mine) {
        game_over(coord);
    } else if (square.adjacent_mines) {
        m_board.get_square(coord).is_revealed = true;
        m_window->update_board(m_board);
    } else {
        m_board.floodfill(coord);
        m_window->update_board(m_board);
    }
}