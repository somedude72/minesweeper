#include <QString>
#include <QPointF>
#include <QMouseEvent>
#include <QFontDatabase>

#include <QAction>
#include <QIcon>
#include <QStyle>
#include <QPixmap>
#include <QMenu>
#include <QWidget>

#include <cmath>
#include <cstddef>
#include <cstdint>

#include "view/game.h"
#include "view/button.h"
#include "model/board.h"
#include "utils/config.h"

GameView::GameView(const GameBoard& init_board, QWidget* parent) : QMainWindow(parent) {
    setupUi(this);
    setupMenu();
    setupFontAndIcons();
    
    connect(window_close, &QPushButton::clicked, this, &GameView::onClose);
    connect(window_min, &QPushButton::clicked, this, &GameView::onMinimize);
    connect(ctrl_button_restart, &QPushButton::clicked, this, &GameView::onRestart);
}

void GameView::setupFontAndIcons() {
    // from https://stackoverflow.com/questions/30973781/qt-add-custom-font-from-resource
    const int32_t window_font_id = QFontDatabase::addApplicationFont(":/assets/window/font.otf");
    const int32_t board_font_id = QFontDatabase::addApplicationFont(":/assets/board/font.ttf");
    m_window_font = QFontDatabase::applicationFontFamilies(window_font_id).at(0);
    m_board_font = QFontDatabase::applicationFontFamilies(board_font_id).at(0);

    window_close->setIcon(QIcon(":/assets/window/close.png"));
    window_min->setIcon(QIcon(":/assets/window/minimize.png"));
    window_close->setIconSize(QSize(10 + 0.5 * std::log(m_min_size), 10 + 0.5 * std::log(m_min_size)));
    window_min->setIconSize(QSize(10 + 0.5 * std::log(m_min_size), 10 + 0.5 * std::log(m_min_size)));
    window_close->setFixedSize(18 + 0.5 * std::log(m_min_size), 18 + 0.5 * std::log(m_min_size));
    window_min->setFixedSize(18 + 0.5 * std::log(m_min_size), 18 + 0.5 * std::log(m_min_size));

    menu_game->setFont(QFont(m_window_font, 12 + 0.4 * std::log(m_min_size)));
    menu_help->setFont(QFont(m_window_font, 12 + 0.4 * std::log(m_min_size)));
    window_title->setFont(QFont(m_window_font, 13 + 0.4 * std::log(m_min_size)));

    ctrl_button_restart->setFixedSize(36 + 0.5 * std::log(m_min_size), 36 + 0.5 * std::log(m_min_size));
    control_widget->setContentsMargins(5, 0, 5, std::log(m_min_size));
    window_bar->adjustSize();

    m_flag.addPixmap(QPixmap(":/assets/board/flag.png"), QIcon::Disabled);
    m_mine.addPixmap(QPixmap(":/assets/board/mine.png"), QIcon::Disabled);
    m_wrong_mine.addPixmap(QPixmap(":/assets/board/cross.png"), QIcon::Disabled);
}

void GameView::setupMenu() {
    QMenu* game_menu_inner = new QMenu(this);
    game_menu_inner->addAction(action_new_game);
    game_menu_inner->addAction(action_about);
    game_menu_inner->addSeparator();
    game_menu_inner->addAction(action_beginner);
    game_menu_inner->addAction(action_intermediate);
    game_menu_inner->addAction(action_expert);
    game_menu_inner->addSeparator();
    game_menu_inner->addAction(action_options);
    menu_game->setMenu(game_menu_inner);
    
    connect(action_new_game, &QAction::triggered, this, &GameView::onRestart);
    connect(action_about, &QAction::triggered, this, &GameView::onActionAbout);
    connect(action_beginner, &QAction::triggered, this, &GameView::onActionBeginner);
    connect(action_intermediate, &QAction::triggered, this, &GameView::onActionIntermediate);
    connect(action_expert, &QAction::triggered, this, &GameView::onActionAdvanced);
    connect(action_options, &QAction::triggered, this, &GameView::onActionOptions);

    QMenu* help_menu_inner = new QMenu(this);
    help_menu_inner->addAction(action_tutorial);
    help_menu_inner->addAction(action_github);
    menu_help->setMenu(help_menu_inner);

    connect(action_tutorial, &QAction::triggered, this, &GameView::onActionTutorial);
    connect(action_github, &QAction::triggered, this, &GameView::onActionGithub);

    menu_bar->layout()->setContentsMargins(6, m_min_size / 450, 12, 0);
}

void GameView::clearBoard() {
    delete board_widget_layout;
    board_widget_layout = new QGridLayout(board_widget);
    board_widget_layout->setSpacing(0);
    board_widget_layout->setObjectName("board_widget_layout");
    board_widget_layout->setContentsMargins(11 + m_min_size / 300, 0, 11 + m_min_size / 300, 12 + m_min_size / 150);

    for (int32_t i = 0; i < m_buttons.size(); i++) {
        for (int32_t j = 0; j < m_buttons[0].size(); j++) {
            delete m_buttons[i][j];
        }
    }
}

void GameView::updateBoard(const GameBoard& board, const GameState& state, bool first_render) {
    updateControlIcon(state);
    assert(board.rowSize() == m_buttons.size() && board.colSize() == m_buttons[0].size());
    for (int32_t i = 0; i < board.rowSize(); i++) {
        for (int32_t j = 0; j < board.colSize(); j++) {
            if (first_render || m_prev_state != state || board.getSquare({ i, j }) != m_prev_board.getSquare({ i, j })) {
                // due to the performance overhead of updating/repainting widgets with
                // stylesheets, we should only update mine squares that have been updated.
                renderButton(board.getSquare({ i, j }), state, m_buttons[i][j]);
            }
        }
    }
    
    m_prev_state = state;
    m_prev_board = board;
}

void GameView::initBoard(const GameBoard& board, const GameState& state, bool first_render) {
    clearBoard();
    m_buttons.assign(board.rowSize(), std::vector<ButtonView*>(board.colSize(), nullptr));
    for (int32_t i = 0; i < board.rowSize(); i++) {
        board_widget_layout->setRowMinimumHeight(i, 26 - 2 * std::log(board.rowSize()));
        for (int32_t j = 0; j < board.colSize(); j++) {
            m_buttons[i][j] = new ButtonView({ i, j }, board_widget);
            const int32_t btn_size = 30 - 2 * std::log(board.rowSize());
            const int32_t icon_size = 27 - 2 * std::log(board.rowSize());
            m_buttons[i][j]->setFixedSize(btn_size, btn_size);
            m_buttons[i][j]->setIconSize(QSize(icon_size, icon_size));
            m_buttons[i][j]->setFont(QFont(m_board_font, m_min_size / 85));

            connect(m_buttons[i][j], &ButtonView::disableSurprisedFace, this, &GameView::onEnableSurpriseFace);
            connect(m_buttons[i][j], &ButtonView::enableSurprisedFace, this, &GameView::onDisableSurpriseFace);
            connect(m_buttons[i][j], &ButtonView::rmbReleasedNormal, this, &GameView::onMark);
            connect(m_buttons[i][j], &ButtonView::lmbReleasedNormal, this, &GameView::onReveal);
            board_widget_layout->addWidget(m_buttons[i][j], i, j);
        }
    }

    updateBoard(board, { false, false }, true);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    LOG_INFO("window: fixed size is {}, {}", size().width(), size().height());
}

void GameView::renderButton(const GameBoardSquare& square, const GameState& new_state, ButtonView* button) const {
    const bool square_revealed = square.is_revealed;
    const bool square_marked = square.is_marked;
    const bool square_has_adj = square.adjacent_mines;
    const bool square_is_mine = square.is_mine;
    const bool square_is_end_reason = square.is_end_reason;
    const QString prev_name = button->objectName();

    if (!square_revealed) {
        button->setObjectName("regular");
        button->setIcon(square_marked ? m_flag : m_no_icon);
        button->setText("");
        button->setDisabled(false);
        button->setClickable(new_state.lost || new_state.won || square_marked ? false : true);
    } else if (square_revealed) {
        if (square_is_mine) {
            button->setObjectName(square_is_end_reason ? "red_background" : "regular"); // For stylesheet
            button->setIcon(m_mine);
            button->setText("");
            button->setDisabled(true);
        } else if (square_has_adj) {
            // the font pack is messed up; the 74th character in the ascii table (which is
            // supposed to be an uppercase j) is the start of the numbers 0-9. therefore,
            // we need to add s.adjacent_mines to 74 to convert it into the appropriate
            // ascii code for the font pack.
            const char16_t temp[2] = { (char16_t) (74 + square.adjacent_mines), '\0' };
            button->setObjectName("mine_" + QString::number(square.adjacent_mines)); // For stylesheet
            button->setIcon(square_marked ? m_wrong_mine : m_no_icon);
            button->setText(square_marked ? "" : QString::fromUtf16(temp));
            button->setDisabled(false);
        } else {
            button->setObjectName("regular");
            button->setIcon(square_marked ? m_wrong_mine : m_no_icon);
            button->setText("");
            button->setDisabled(true);
        }
    }

    if (button->objectName() != prev_name)
        button->style()->polish(button);
}

void GameView::setMinesLeft(int new_mines) {
    mine_display->display(new_mines);
}

void GameView::setTimePassed(int new_time) {
    timer_display->display(new_time);
}

void GameView::onRestart() const {
    emit restart();
}

void GameView::onReveal(const GameBoardCoord& coord) const {
    emit reveal(coord);
}

void GameView::onMark(const GameBoardCoord& coord) const {
    emit mark(coord);
}

void GameView::onClose() const {
    emit close();
}

void GameView::onMinimize() const {
    emit minimize();
}

void GameView::onActionTutorial() const {
    emit actionTutorial();
}

void GameView::onActionGithub() const {
    emit actionGithub();
}

void GameView::onEnableSurpriseFace(const GameBoardCoord& coord) {
    if (!m_prev_state.won && !m_prev_state.lost) {
        m_prev_state.revealing_mine = true;
        updateControlIcon(m_prev_state);
    }
}

void GameView::onDisableSurpriseFace(const GameBoardCoord& coord) {
    if (!m_prev_state.won && !m_prev_state.lost) {
        m_prev_state.revealing_mine = false;
        updateControlIcon(m_prev_state);
    }
}

void GameView::onActionBeginner() const {
    emit actionBeginner();
}

void GameView::onActionIntermediate() const {
    emit actionIntermediate();
}

void GameView::onActionAdvanced() const {
    emit actionAdvanced();
}

void GameView::onActionAbout() const {
    emit actionAbout();
}

void GameView::onActionOptions() const {
    emit actionOptions();
}

void GameView::updateControlIcon(const GameState& state) const {
    if (state.won) {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/win.png"));
    } else if (state.lost) {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/dead.png"));
    } else if (state.revealing_mine) {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/revealing.png"));
    } else {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/smile.png"));
    }
}

void GameView::mousePressEvent(QMouseEvent* event) {
    QWidget* child = childAt(event->position().toPoint());
    if (child == window_bar || child == window_title) {
        m_prev_position = event->globalPosition();
        m_moving_window = true;
    }
}

void GameView::mouseMoveEvent(QMouseEvent* event) {
    if (m_moving_window) {
        const QPointF delta = event->globalPosition() - m_prev_position;
        move(x() + delta.x(), y() + delta.y());
        m_prev_position = event->globalPosition();
    }
}

void GameView::mouseReleaseEvent(QMouseEvent* event) {
    m_moving_window = false;
}