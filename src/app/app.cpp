#include <cstdint>
#include <string>

#include <QApplication>
#include <QStyle>
#include <fmt/format.h>

#include "app/app.h"
#include "view/about.h"
#include "view/window.h"
#include "model/data.h"

#include "utils/config.h"

// {0} = thick border size
// {1} = thin border size
// {2} = vertical spacing (control widget)
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
	margin: 4px 11px 11px 11px;
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
	margin-top: 12px;
	margin-bottom: 17px;
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

QLabel#window_title {{
    margin: 0px 0px 2px 2px;
    color: rgb(255, 255, 255);
}}

.QToolButton::menu-indicator {{
    image: none;
}}

.QToolButton {{
    margin-top: 2px;
    border: 0px solid black;
    color: black;
}}

.QToolButton:pressed {{
    padding: 0px;
    background-color: rgb(245, 245, 245);
    color: black;
}}
)";

App::App(int argc, char** argv) : QApplication(argc, argv) {
    SET_LOG_PRIORITY(DEBUG_LEVEL); // this is unused lol
    SET_LOG_PATTERN("[%a, %b %d %H:%M:%S] [%l] %v");
    LOG_INFO("app: logging initiated (this is a test message)");

    setStyleSheet(QString::fromStdString(fmt::format(app_style,
        min_size / 300,
        min_size / 450,
        min_size / 450
    )));

    m_game_over = false;
    m_game_won = false;

    m_settings = model::GameSettings();
    m_board = model::MineBoard(m_settings);
    m_board.setupBoard();
    m_game_window = new view::GameWindow(m_board);
    m_game_window->setWindowFlags(Qt::FramelessWindowHint);
    m_game_window->initBoard(m_board, { false, false }, true);
    m_game_window->show();
    
    connect(m_game_window, &view::GameWindow::restart, this, &App::onRestart);
    connect(m_game_window, &view::GameWindow::reveal, this, &App::onReveal);
    connect(m_game_window, &view::GameWindow::mark, this, &App::onMark);
    connect(m_game_window, &view::GameWindow::actionAbout, this, &App::onActionAbout);
    connect(m_game_window, &view::GameWindow::actionBeginner, this, &App::onActionBeginner);
    connect(m_game_window, &view::GameWindow::actionIntermediate, this, &App::onActionIntermediate);
    connect(m_game_window, &view::GameWindow::actionAdvanced, this, &App::onActionAdvanced);

    connect(m_game_window, &view::GameWindow::close, this, &App::onClose);
    connect(m_game_window, &view::GameWindow::minimize, this, &App::onMinimize);
}

App::~App() {
    delete m_game_window;
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
    if (m_board.rowSize() == m_settings.row_size && m_board.colSize() == m_settings.col_size) {
        m_board = model::MineBoard(m_settings);
        m_board.setupBoard();
        m_game_window->updateBoard(m_board, { m_game_won, m_game_over, false });
    } else {
        m_board = model::MineBoard(m_settings);
        m_board.setupBoard();
        m_game_window->initBoard(m_board, { m_game_won, m_game_over, false });
    }
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
    m_game_window->updateBoard(m_board, { m_game_won, m_game_over, false });
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
        m_game_window->updateBoard(m_board, { m_game_won, m_game_over, false });
    } else if (!square.is_revealed) {
        m_board.floodfill(coord);
        m_game_window->updateBoard(m_board, { m_game_won, m_game_over, false });
    } else {
        bool is_mine = m_board.revealAdjacent(coord);
        if (is_mine) revealMines(coord);
        m_game_over = is_mine;
        m_game_window->updateBoard(m_board, { m_game_won, m_game_over, false });
    }

    if (m_board.didWin()) {
        m_game_won = true;
        m_game_window->updateBoard(m_board, { m_game_won, m_game_over, false });
    }
}

void App::deleteAboutWindow() {
    m_about_window->deleteLater();
    m_about_window = nullptr;
}

void App::onActionAbout() {
    m_about_window = new view::AboutWindow();
    connect(m_about_window, &view::AboutWindow::finished, this, &App::deleteAboutWindow);
    m_about_window->exec();
}

void App::onActionBeginner() {
    m_settings.row_size = 9;
    m_settings.col_size = 9;
    m_settings.num_mines = 10;
    onRestart();
}

void App::onActionIntermediate() {
    m_settings.row_size = 15;
    m_settings.col_size = 15;
    m_settings.num_mines = 40;
    onRestart();
}

void App::onActionAdvanced() {
    m_settings.row_size = 16;
    m_settings.col_size = 30;
    m_settings.num_mines = 99;
    onRestart();
}

void App::onMinimize() {
    m_game_window->setWindowState(Qt::WindowMinimized);
}

void App::onClose() {
    this->exit(0);
}