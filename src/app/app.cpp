#include <cstdint>
#include <string>

#include "QApplication"
#include "QStyle"
#include "fmt/format.h"

#include "app/app.h"
#include "model/screen.h"
#include "view/window.h"
#include "model/data.h"
#include "utils/config.h"

// {0} = thick border size
// {1} = thin border size
// {2} = font size
static const std::string app_style = R"(
QWidget#root_grid {{
	border: 0px solid gray;
    border-right: {0}px solid gray;
	border-bottom: {0}px solid gray;
	border-top: {0}px solid white;
	border-left: {0}px solid white;
    background-color: rgb(205, 205, 205);
}}

QWidget#board_widget {{
	margin: 0px 11px 11px 11px;
	border: 0px solid gray;
    border-top: {0}px solid gray;
	border-left: {0}px solid gray;
	border-right: {0}px solid white;
	border-bottom: {0}px solid white;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}

QWidget#control_widget {{
	margin: 11px 11px 11px 11px;
	border: 0px solid gray;
    border-top: {0}px solid gray;
	border-left: {0}px solid gray;
	border-right: {0}px solid white;
	border-bottom: {0}px solid white;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}

QPushButton#ctrl_button_restart {{
    border: 0px solid gray;
    border-right: {0}px solid gray;
	border-bottom: {0}px solid gray;
	border-top: {0}px solid white;
	border-left: {0}px solid white;
    background-color: rgb(205, 205, 205);
	margin-top: 19px;
	margin-bottom: 19px;
    color: rgb(0, 0, 0);
}}
        
QPushButton#ctrl_button_restart:pressed {{
	border: 0px solid gray;
    border-right: {0}px solid white;
	border-bottom: {0}px solid white;
	border-top: {0}px solid gray;
	border-left: {0}px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}

QWidget#window_bar {{
    margin: 4px 6px 0px 6px;
    background-color: rgb(0, 0, 148);
}}

QPushButton#window_close {{
    border: 0px solid gray;
    border-right: {1}px solid gray;
	border-bottom: {1}px solid gray;
	border-top: {1}px solid white;
	border-left: {1}px solid white;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}
        
QPushButton#window_close:pressed {{
	border: 0px solid gray;
    border-right: {1}px solid white;
	border-bottom: {1}px solid white;
	border-top: {1}px solid gray;
	border-left: {1}px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}

QPushButton#window_min {{
    border: 0px solid gray;
    border-right: {1}px solid gray;
	border-bottom: {1}px solid gray;
	border-top: {1}px solid white;
	border-left: {1}px solid white;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}
        
QPushButton#window_min:pressed {{
	border: 0px solid gray;
    border-right: {1}px solid white;
	border-bottom: {1}px solid white;
	border-top: {1}px solid gray;
	border-left: {1}px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}

.QToolButton {{
    margin-top: 2px;
    border: 0px solid black;
    color: black;
}}

.QToolButton:pressed {{
    padding: 0px;
    background-color: rgb(245, 245, 245);
    font-size: {2};
    color: black;
}}

.QToolButton::menu-indicator {{
    image: none;
}}

QLabel#window_title {{
    margin: 0px 0px 2px 2px;
    color: rgb(255, 255, 255);
}}
)";

App::App(int argc, char** argv) : QApplication(argc, argv) {
    SET_LOG_PRIORITY(DEBUG_LEVEL);
    SET_LOG_PATTERN("[%a, %b %d %H:%M:%S] [%l] %v");
    LOG_INFO("app: logging initiated (this is a test message)");

    m_game_over = false;
    m_game_won = false;

    m_board = model::MineBoard(30, 50);
    m_window = new view::MineWindow(m_board);
    m_window->setWindowFlags(Qt::FramelessWindowHint);
    m_window->show();

    const int32_t min_size = model::Screen::getMinSize();
    setStyleSheet(QString::fromStdString(fmt::format(app_style,
        min_size / 300,
        min_size / 450,
        min_size / 75
    )));
    
    connect(m_window, &view::MineWindow::restart, this, &App::onRestart);
    connect(m_window, &view::MineWindow::reveal, this, &App::onReveal);
    connect(m_window, &view::MineWindow::mark, this, &App::onMark);
    
    connect(m_window, &view::MineWindow::close, this, &App::onClose);
    connect(m_window, &view::MineWindow::minimize, this, &App::onMinimize);
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
    if (!square.is_revealed)
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