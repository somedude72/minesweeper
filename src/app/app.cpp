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

    m_board = model::MineBoard(30, 50);
    m_window = new view::MineWindow(m_board);
    m_window->setWindowFlags(Qt::FramelessWindowHint);

    connect(m_window, &view::MineWindow::restart, this, &App::onRestart);
    connect(m_window, &view::MineWindow::reveal, this, &App::onReveal);
    connect(m_window, &view::MineWindow::mark, this, &App::onMark);

    connect(m_window, &view::MineWindow::close, this, &App::onClose);
    connect(m_window, &view::MineWindow::minimize, this, &App::onMinimize);
    m_window->show();
}

App::~App() {
    delete m_window;
    LOG_INFO("app: deallocated window object");
    LOG_INFO("app: terminating application");
}

void App::revealMines(const model::MineCoord& last_reveal) {
    for (int32_t i = 0; i < m_board.rowSize(); i++) {
        for (int32_t j = 0; j < m_board.colSize(); j++) {
            if (!m_board.getSquare({ i, j }).is_mine && !m_board.getSquare({ i, j }).is_marked)
                continue;
            if (i == last_reveal.row && j == last_reveal.col) {
                m_board.getSquare({ i, j }).is_revealed = true;
                m_board.getSquare({ i, j }).is_end_reason = true;
            } else if (!m_board.getSquare({ i, j }).is_mine && m_board.getSquare({ i, j }).is_marked) {
                m_board.getSquare({ i, j }).is_revealed = true; // wrongly marked
            } else if (m_board.getSquare({ i, j }).is_mine && !m_board.getSquare({ i, j }).is_marked) {
                m_board.getSquare({ i, j }).is_revealed = true; // not marked
            }
        }
    }
}

void App::onRestart() {
    LOG_INFO("app: received restart game signal");
    m_game_over = false;
    m_game_won = false;
    m_board = model::MineBoard(30, 50);
    m_window->updateWindow(m_board, { m_game_won, m_game_over, false });
}

void App::onMark(const model::MineCoord& coord) {
    if (m_game_over || m_game_won) {
        LOG_DEBUG("skipping mark due to game_over || game_won");
        return;
    }

    LOG_INFO("app: received mark signal at ({}, {})", coord.row, coord.col);
    model::MineSquare& square = m_board.getSquare(coord);
    square.is_marked = !square.is_marked;
    m_window->updateWindow(m_board, { m_game_won, m_game_over, false });
}

void App::onReveal(const model::MineCoord& coord) {
    if (m_game_over || m_game_won || m_board.getSquare(coord).is_marked) {
        LOG_DEBUG("skipping reveal due to game_over || game_won || is_marked");
        return;
    }

    LOG_INFO("app: received reveal signal at ({}, {})", coord.row, coord.col);
    model::MineSquare& square = m_board.getSquare(coord);

    if (square.is_mine) {
        revealMines(coord);
        m_game_over = true;
        m_window->updateWindow(m_board, { m_game_won, m_game_over, false });
    } else if (!square.is_revealed) {
        m_board.floodfill(coord);
        m_window->updateWindow(m_board, { m_game_won, m_game_over, false });
    } else {
        bool is_mine = m_board.revealAdjacent(coord);
        if (is_mine) revealMines(coord);
        m_game_over = is_mine;
        m_window->updateWindow(m_board, { m_game_won, m_game_over, false });
    }

    if (m_board.didWin()) {
        m_game_won = true;
        m_window->updateWindow(m_board, { m_game_won, m_game_over, false });
    }
}

void App::onMinimize() {
    m_window->setWindowState(Qt::WindowMinimized);
}

void App::onClose() {
    this->exit(0);
}