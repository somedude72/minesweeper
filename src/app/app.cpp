#include <QApplication>
#include <QDesktopServices>
#include <QTimer>
#include <QUrl>
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

#main_separator {{
    background-color: rgb(100, 100, 100);
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
    std::srand(std::time(nullptr)); // seed generation
    m_settings.seed = (m_settings.is_set_seed) ? m_settings.seed : std::rand();

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
    connect(m_game_window, &GameView::revealAltDown, this, &App::onRevealAltDown);
    connect(m_game_window, &GameView::revealAltUp, this, &App::onRevealAltUp);
    connect(m_game_window, &GameView::actionAbout, this, &App::onActionAbout);
    connect(m_game_window, &GameView::actionBeginner, this, &App::onActionBeginner);
    connect(m_game_window, &GameView::actionIntermediate, this, &App::onActionIntermediate);
    connect(m_game_window, &GameView::actionAdvanced, this, &App::onActionAdvanced);
    connect(m_game_window, &GameView::actionOptions, this, &App::onActionOptions);
    connect(m_game_window, &GameView::actionGithub, this, &App::onActionGithub);
    connect(m_game_window, &GameView::actionTutorial, this, &App::onActionTutorial);

    connect(m_game_window, &GameView::close, this, &App::onClosePressed);
    connect(m_game_window, &GameView::minimize, this, &App::onMinimizePressed);
}

App::~App() {
    delete m_game_window;
    LOG_DEBUG("app: deallocated window object");
    LOG_DEBUG("app: terminated event loop");
}

void App::setupLCD() {
    delete m_timer;
    m_timer = new QTimer(this);
    m_timer->callOnTimeout(this, &App::onTimerUpdated);
    m_state.timer = 0;
    m_state.mines = m_settings.num_mines;
    m_game_window->setMinesLeft(m_state.mines);
    m_game_window->setTimePassed(m_state.timer);
}

void App::onTimerUpdated() {
    if (!m_state.won && !m_state.lost) {
        m_state.timer++;
        m_game_window->setTimePassed(m_state.timer);
        m_timer->start(1000);
    }
}

void App::onRestart() {
    setupLCD();
    m_state.lost = false;
    m_state.won = false;
    m_state.revealing_mine = false;
    m_state.is_first_reveal = true;
    m_settings.seed = (m_settings.is_set_seed) ? m_settings.seed : std::rand();

    if (m_board.rowSize() == m_settings.row_size
        && m_board.colSize() == m_settings.col_size) {
        m_board.reset(m_settings);
        m_game_window->updateBoard(m_board, m_state);
    } else {
        m_board.reset(m_settings);
        m_game_window->initBoard(m_board, m_state);
    }
}

void App::onMark(const GameBoardCoord& coord) {
    m_board.mark(coord, m_state);
    m_game_window->updateBoard(m_board, m_state);
    m_game_window->setMinesLeft(m_state.mines);
}

void App::onReveal(const GameBoardCoord& coord) {
    if (m_state.is_first_reveal) {
        m_board.generateMines(coord);
        m_timer->start(1000);
    }

    m_board.reveal(coord, m_state);
    m_game_window->updateBoard(m_board, m_state);
}

void App::onRevealAltDown(const GameBoardCoord& coord) {
    m_state.revealing_mine = true;
    m_board.revealAdjacentDown(coord);
    m_game_window->updateBoard(m_board, m_state);
}

void App::onRevealAltUp(const GameBoardCoord& coord) {
    m_state.revealing_mine = false;
    m_board.revealAdjacentUp();
    m_game_window->updateBoard(m_board, m_state);
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
    OptionsView* window = new OptionsView(m_settings, m_game_window);
    connect(window, &OptionsView::applySettings, this, &App::onOptionsChanged);
    window->setAttribute(Qt::WA_DeleteOnClose); // makes it so that we don't have to manually
    window->exec();                             // delete the window after it closes
}

void App::onActionGithub() const {
    QDesktopServices::openUrl(QUrl("https://github.com/somedude72/minesweeper"));
}

void App::onActionTutorial() const {
    QDesktopServices::openUrl(QUrl("https://github.com/somedude72/minesweeper/blob/main/docs/tutorial.md"));
}

void App::onActionAbout() {
    AboutView* window = new AboutView(m_game_window);
    window->setAttribute(Qt::WA_DeleteOnClose); // makes it so that we don't have to manually
    window->exec();                             // delete the window after it closes
}

void App::onOptionsChanged(const GameSettings& new_settings) {
    const GameSettings old = m_settings;
    m_settings = new_settings;
    m_board.updateSettings(new_settings);
    if (old.col_size != m_settings.col_size
        || old.row_size != m_settings.row_size
        || old.num_mines != m_settings.num_mines) {
        onRestart();
    }
}

void App::onMinimizePressed() {
    m_game_window->setWindowState(Qt::WindowMinimized);
}

void App::onClosePressed() {
    exit(0);
}