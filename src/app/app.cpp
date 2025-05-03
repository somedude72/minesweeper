#include <cstdint>

#include <QApplication>
#include <QTimer>
#include <fmt/format.h>

#include "app/app.h"
#include "view/game.h"
#include "view/about.h"
#include "view/options.h"
#include "model/board.h"
#include "utils/config.h"

// {0} = thick border size
// {1} = thin border size
// {2} = vertical spacing (control widget)
static constexpr const char* app_style = R"(
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

.QLCDNumber {{ 
    border: 2px rgb(0, 0, 0); 
    background-color: black; 
    color: red; 
}}
)";

App::App(int argc, char** argv) : QApplication(argc, argv) {
    SET_LOG_PRIORITY(DEBUG_LEVEL);
    SET_LOG_PATTERN("[%a, %b %d %H:%M:%S] [%l] %v");
    LOG_DEBUG("app: logging initiated (this is a test message)");

    setStyleSheet(QString::fromStdString(fmt::format(
        app_style,
        min_size / 300,
        min_size / 450,
        min_size / 450
    )));

    m_settings = GameSettings();
    m_state = GameState();
    m_board = GameBoard(m_settings);
    m_game_window = new GameView(m_board);
    m_game_window->setWindowFlags(Qt::FramelessWindowHint);
    m_game_window->initBoard(m_board, m_state, true);
    m_game_window->show();

    setupLCD();
    
    // registering events (signal/slots)
    connect(m_game_window, &GameView::restart, this, &App::onRestart);
    connect(m_game_window, &GameView::reveal, this, &App::onReveal);
    connect(m_game_window, &GameView::mark, this, &App::onMark);
    connect(m_game_window, &GameView::actionAbout, this, &App::onActionAbout);
    connect(m_game_window, &GameView::actionBeginner, this, &App::onActionBeginner);
    connect(m_game_window, &GameView::actionIntermediate, this, &App::onActionIntermediate);
    connect(m_game_window, &GameView::actionAdvanced, this, &App::onActionAdvanced);
    connect(m_game_window, &GameView::actionOptions, this, &App::onActionOptions);

    connect(m_game_window, &GameView::close, this, &App::onClosePressed);
    connect(m_game_window, &GameView::minimize, this, &App::onMinimizePressed);
}

App::~App() {
    delete m_game_window;
    LOG_DEBUG("app: deallocated window object");
    LOG_DEBUG("app: terminated event loop");
}

void App::gameOverRevealMines(const GameBoardCoord& last_reveal) {
    for (int32_t i = 0; i < m_board.rowSize(); i++) {
        for (int32_t j = 0; j < m_board.colSize(); j++) {
            if (!m_board.getSquare({ i, j }).is_mine && !m_board.getSquare({ i, j }).is_marked)
                continue; // we do not want to do anything with marked mines
            if (i == last_reveal.row && j == last_reveal.col) {
                m_board.getSquare({ i, j }).is_revealed = true;
                m_board.getSquare({ i, j }).is_end_reason = true;
            } else if (!m_board.getSquare({ i, j }).is_mine && m_board.getSquare({ i, j }).is_marked) {
                m_board.getSquare({ i, j }).is_revealed = true; // wrongly marked mine
            } else if (m_board.getSquare({ i, j }).is_mine && !m_board.getSquare({ i, j }).is_marked) {
                m_board.getSquare({ i, j }).is_revealed = true; // not marked mine
            }
        }
    }
}

void App::setupLCD() {
    m_state.timer = 0;
    m_state.mines = m_settings.num_mines;
    m_game_window->setMinesLeft(m_state.mines);
    m_game_window->setTimePassed(m_state.timer);
    delete m_timer;
    m_timer = new QTimer(this);
    m_timer->callOnTimeout(this, &App::onTimerUpdated);
}

void App::onTimerUpdated() {
    if (!m_state.won && !m_state.lost) {
        m_state.timer++;
        m_game_window->setTimePassed(m_state.timer);
        m_timer->start(1000);
    }
}

void App::onRestart() {
    m_state.lost = false;
    m_state.won = false;
    m_state.is_first_reveal = true;
    setupLCD();
    if (m_board.rowSize() == m_settings.row_size && m_board.colSize() == m_settings.col_size) {
        m_board = GameBoard(m_settings);
        m_game_window->updateBoard(m_board, m_state);
    } else {
        m_board = GameBoard(m_settings);
        m_game_window->initBoard(m_board, m_state);
    }
}

void App::onMark(const GameBoardCoord& coord) {
    if (m_state.lost || m_state.won || m_board.getSquare(coord).is_revealed)
        return;
    if (m_board.getSquare(coord).is_marked) {
        m_board.getSquare(coord).is_marked = false;
        m_state.mines++;
    } else if (m_state.mines != 0) {
        m_board.getSquare(coord).is_marked = true;
        m_state.mines--;
    } else {
        return;
    }

    m_game_window->updateBoard(m_board, m_state);
    m_game_window->setMinesLeft(m_state.mines);
}

void App::onReveal(const GameBoardCoord& coord) {
    if (m_state.lost || m_state.won || m_board.getSquare(coord).is_marked)
        return;
    if (m_state.is_first_reveal) {
        m_board.generateMines(coord);
        m_timer->start(1000);
    }

    if (m_board.getSquare(coord).is_mine) {
        gameOverRevealMines(coord);
        m_state.lost = true;
        m_game_window->updateBoard(m_board, m_state);
    } else if (!m_board.getSquare(coord).is_revealed) {
        m_board.floodfill(coord);
        m_game_window->updateBoard(m_board, m_state);
    } else {
        bool is_mine = m_board.revealAdjacent(coord);
        if (is_mine) gameOverRevealMines(coord);
        m_state.lost = is_mine;
        m_game_window->updateBoard(m_board, m_state);
    }

    if (m_board.didWin()) {
        m_state.won = true;
        m_game_window->updateBoard(m_board, m_state);
    }

    m_state.is_first_reveal = false;
}

void App::onRevealAltDown(const GameBoardCoord& coord) { }

void App::onRevealAltUp(const GameBoardCoord& coord) { }

void App::onActionAbout() {
    AboutView* w = new AboutView(m_game_window);
    w->setAttribute(Qt::WA_DeleteOnClose);
    w->exec();
}

void App::onActionBeginner() {
    m_settings.row_size = 9;
    m_settings.col_size = 9;
    m_settings.num_mines = 10;
    onRestart();
}

void App::onActionIntermediate() {
    m_settings.row_size = 12;
    m_settings.col_size = 20;
    m_settings.num_mines = 40;
    onRestart();
}

void App::onActionAdvanced() {
    m_settings.row_size = 16;
    m_settings.col_size = 30;
    m_settings.num_mines = 99;
    onRestart();
}

void App::onActionOptions() const {
    OptionsView* o = new OptionsView(m_settings, m_game_window);
    connect(o, &OptionsView::applySettings, this, &App::onOptionsChanged);
    o->setAttribute(Qt::WA_DeleteOnClose);
    o->exec();
}

void App::onOptionsChanged(const GameSettings& settings) {
    m_settings = settings;
    onRestart();
}

void App::onMinimizePressed() {
    m_game_window->setWindowState(Qt::WindowMinimized);
}

void App::onClosePressed() {
    exit(0);
}