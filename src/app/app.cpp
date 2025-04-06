#include "QApplication"

#include "app/app.h"
#include "view/window.h"
#include "model/data.h"
#include "utils/config.h"

App::App(int argc, char** argv) : QApplication(argc, argv) {
    SET_LOG_PRIORITY(DEBUG_LEVEL);
    SET_LOG_PATTERN("[%a, %b %d %H:%M:%S] [%l] %v");
    LOG_INFO("app: logging initiated (this is a test message)");
    
    m_game_over = false;
    m_game_won = false;

    m_board = model::MineBoard(9, 9);
    m_window = new view::MineWindow(m_board);
    m_window->setWindowFlags(Qt::FramelessWindowHint);

    connect(m_window, &view::MineWindow::restart, this, &App::on_restart);
    connect(m_window, &view::MineWindow::reveal, this, &App::on_reveal);
    connect(m_window, &view::MineWindow::mark, this, &App::on_mark);
    connect(m_window, &view::MineWindow::close, this, &App::on_close);

    m_window->show();
}

App::~App() {
    delete m_window;
    LOG_INFO("app: deallocated window object");
    LOG_INFO("app: terminating application");
}

void App::reveal_mines(const model::MineCoord& last_reveal) {
    for (int32_t i = 0; i < m_board.row_size(); i++) {
        for (int32_t j = 0; j < m_board.col_size(); j++) {
            if (!m_board.get_square({ i, j }).is_mine)
                continue;
            if (i != last_reveal.row || j != last_reveal.col) {
                m_board.get_square({ i, j }).is_revealed = true;
            } else {
                m_board.get_square({ i, j }).is_end_reason = true;
                m_board.get_square({ i, j }).is_revealed = true;
            }
        }
    }
}

void App::on_restart() {
    LOG_INFO("app: received restart game signal");

    m_game_over = false;
    m_game_won = false;

    m_board = model::MineBoard(9, 9);
    m_window->render_board(m_board, m_game_over, m_game_won);
}

void App::on_mark(const model::MineCoord& coord) {
    if (m_game_over || m_game_won) {
        LOG_DEBUG("skipping mark due to game_over || game_won");
        return;
    }

    LOG_INFO("app: received mark signal at ({}, {})", coord.row, coord.col);
    
    model::MineSquare& square = m_board.get_square(coord);
    square.is_marked = !square.is_marked;
    m_window->render_board(m_board, m_game_over, m_game_won);
}

void App::on_reveal(const model::MineCoord& coord) {
    if (m_game_over || m_game_won || m_board.get_square(coord).is_marked) {
        LOG_DEBUG("skipping reveal due to game_over || game_won || is_marked");
        return;
    }

    LOG_INFO("app: received reveal signal at ({}, {})", coord.row, coord.col);
    model::MineSquare& square = m_board.get_square(coord);

    if (square.is_mine) {
        reveal_mines(coord);
        m_game_over = true;
        m_window->render_board(m_board, m_game_over, m_game_won);
    } else if (square.adjacent_mines) {
        m_board.get_square(coord).is_revealed = true;
        m_window->render_board(m_board, m_game_over, m_game_won);
    } else {
        m_board.floodfill(coord);
        m_window->render_board(m_board, m_game_over, m_game_won);
    }

    if (m_board.did_win()) {
        m_game_won = true;
        m_window->render_board(m_board, m_game_over, m_game_won);
    }
}

void App::on_close() {
    this->exit();
}