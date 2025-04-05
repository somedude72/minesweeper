#include "QApplication"

#include "app/app.h"
#include "view/game.h"
#include "model/data.h"
#include "utils/config.h"

App::App(int argc, char** argv) : QApplication(argc, argv) {
    SET_LOG_PRIORITY(DEBUG_LEVEL);
    SET_LOG_PATTERN("[%a, %b %d %H:%M:%S] [%l] %v");
    LOG_INFO("app: logging initiated (this is a test message)");
    
    m_board = model::MineBoard(10, 15);
    m_window = new view::MineWindow(m_board);
    m_window->setWindowFlags(Qt::FramelessWindowHint);

    is_game_over = false;

    connect(m_window, &view::MineWindow::restart, this, &App::on_restart);
    connect(m_window, &view::MineWindow::reveal, this, &App::on_reveal);
    connect(m_window, &view::MineWindow::mark, this, &App::on_mark);

    m_window->show();
}

App::~App() {
    delete m_window;
    LOG_INFO("app: deallocated window object");
    LOG_INFO("app: terminating application");
}

void App::game_over(const model::MineCoord& cause) {
    LOG_INFO("app: game over due to revealing mine");
    for (int32_t i = 0; i < m_board.row_size(); i++) {
        for (int32_t j = 0; j < m_board.col_size(); j++) {
            if (!m_board.get_square({ i, j }).is_mine)
                continue;
            if (i != cause.row || j != cause.col) {
                m_board.get_square({ i, j }).is_revealed = true;
            } else {
                m_board.get_square({ i, j }).is_revealed = true;
                m_board.get_square({ i, j }).is_end_reason = true;
            }
        }
    }

    m_window->update_board(m_board, true, false);
}

void App::on_restart() {
    LOG_INFO("app: received restart game signal");

    m_board = model::MineBoard(10, 15);
    m_window->update_board(m_board, false, false);
    is_game_over = false;
}

void App::on_mark(const model::MineCoord& coord) {
    if (is_game_over) {
        return;
    }

    LOG_INFO("app: received mark signal at ({}, {})", coord.row, coord.col);
    model::MineSquare& square = m_board.get_square(coord);
    
    square.is_marked = !square.is_marked;
    m_window->update_board(m_board, false, false);
}

void App::on_reveal(const model::MineCoord& coord) {
    if (is_game_over) {
        return;
    }

    LOG_INFO("app: received reveal signal at ({}, {})", coord.row, coord.col);
    model::MineSquare& square = m_board.get_square(coord);

    if (square.is_mine) {
        game_over(coord);
        is_game_over = true;
        return;
    } else if (square.adjacent_mines) {
        m_board.get_square(coord).is_revealed = true;
        m_window->update_board(m_board, false, false);
    } else {
        m_board.floodfill(coord);
        m_window->update_board(m_board, false, false);
    }

    if (m_board.did_win()) {
        m_window->update_board(m_board, false, true);
    }
}