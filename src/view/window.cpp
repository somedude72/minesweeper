#include "view/window.h"
#include "view/button.h"
#include "model/data.h"
#include "utils/config.h"

#include <QString>
#include <QPoint>
#include <QMouseEvent>
#include <QAction>
#include <QMenu>

#include <QGuiApplication>
#include <QFontDatabase>
#include <QIcon>
#include <QStyle>
#include <QPixmap>

#include <cstdint>

namespace view {

GameWindow::GameWindow(const model::MineBoard& init_board, QWidget* parent) : QMainWindow(parent) {
    LOG_INFO("window: initializing main ui");
    setupUi(this);
    setupMenu();
    setupFontAndIcons();
    
    connect(window_close, &QPushButton::clicked, this, &GameWindow::onClose);
    connect(window_min, &QPushButton::clicked, this, &GameWindow::onMinimize);
    connect(ctrl_button_restart, &QPushButton::clicked, this, &GameWindow::onRestart);
}

void GameWindow::setupFontAndIcons() {
    // From https://stackoverflow.com/questions/30973781/qt-add-custom-font-from-resource
    const int32_t window_font_id = QFontDatabase::addApplicationFont(":/assets/window/font.otf");
    const int32_t board_font_id = QFontDatabase::addApplicationFont(":/assets/board/font.ttf");
    m_window_font = QFontDatabase::applicationFontFamilies(window_font_id).at(0);
    m_board_font = QFontDatabase::applicationFontFamilies(board_font_id).at(0);

    window_close->setIcon(QIcon(":/assets/window/close.png"));
    window_min->setIcon(QIcon(":/assets/window/minimize.png"));
    window_title->setFont(QFont(m_window_font, 16));
    
    menu_game->setFont(QFont(m_window_font, 14));
    menu_help->setFont(QFont(m_window_font, 14));

    m_flag.addPixmap(QPixmap(":/assets/board/flag.png"), QIcon::Disabled);
    m_flag.addPixmap(QPixmap(":/assets/board/flag.png"), QIcon::Normal);
    m_mine.addPixmap(QPixmap(":/assets/board/mine.png"), QIcon::Disabled);
    m_mine.addPixmap(QPixmap(":/assets/board/mine.png"), QIcon::Normal);
    m_wrong_mine.addPixmap(QPixmap(":/assets/board/cross.png"), QIcon::Disabled);
    m_wrong_mine.addPixmap(QPixmap(":/assets/board/cross.png"), QIcon::Normal);

    window_close->setFixedSize(21, 21);
    window_min->setFixedSize(21, 21);
    window_close->setIconSize(QSize(13, 13));
    window_min->setIconSize(QSize(13, 13));
    window_bar->adjustSize();
}

void GameWindow::setupMenu() {
    QMenu* game_menu_inner = new QMenu(this);
    game_menu_inner->addAction(action_new_game);
    game_menu_inner->addAction(action_about);
    QAction* separator_game = game_menu_inner->addSeparator();
    game_menu_inner->addAction(action_beginner);
    game_menu_inner->addAction(action_intermediate);
    game_menu_inner->addAction(action_advanced);
    QAction* separator_adv = game_menu_inner->addSeparator();
    game_menu_inner->addAction(action_safe_first_click);
    game_menu_inner->addAction(action_custom);
    separator_game->setToolTip("Game");
    separator_adv->setWhatsThis("Advanced");

    menu_game->setMenu(game_menu_inner);
    menu_bar->layout()->setContentsMargins(6, m_min_size / 450, 12, 0);
    
    connect(action_new_game, &QAction::triggered, this, &GameWindow::onRestart);
    connect(action_about, &QAction::triggered, this, &GameWindow::onActionAbout);
    connect(action_beginner, &QAction::triggered, this, &GameWindow::onActionBeginner);
    connect(action_intermediate, &QAction::triggered, this, &GameWindow::onActionIntermediate);
    connect(action_advanced, &QAction::triggered, this, &GameWindow::onActionAdvanced);
    connect(action_custom, &QAction::triggered, this, &GameWindow::onActionCustom);
    connect(action_safe_first_click, &QAction::triggered, this, &GameWindow::onActionSetSafeFirstClick);
}

void GameWindow::updateBoard(const model::MineBoard& new_board, const model::GameState& new_state, bool first_render) {
    updateControlIcon(new_state);
    assert(new_board.rowSize() == m_buttons.size() && new_board.colSize() == m_buttons[0].size());
    for (int32_t i = 0; i < new_board.rowSize(); i++) {
        for (int32_t j = 0; j < new_board.colSize(); j++) {
            if (m_prev_state != new_state || first_render || 
                new_board.getSquare({ i, j }) != m_prev_board.getSquare({ i, j })) {
                // Due to the performance overhead of updating/repainting widgets with
                // stylesheets, we should only update mine squares that have been updated.
                renderButton(new_board.getSquare({ i, j }), new_state, m_buttons[i][j]);
            }
        }
    }
    
    m_prev_state = new_state;
    m_prev_board = new_board;
}

void GameWindow::initBoard(const model::MineBoard& new_board, const model::GameState& new_state, bool first_render) {
    for (int32_t i = 0; i < m_buttons.size(); i++)
        for (int32_t j = 0; j < m_buttons[0].size(); j++)
            delete m_buttons[i][j];
    delete board_widget_layout;
    board_widget_layout = new QGridLayout(board_widget);
    board_widget_layout->setSpacing(0);
    board_widget_layout->setObjectName("board_widget_layout");
    board_widget_layout->setContentsMargins(11 + m_min_size / 300, m_min_size / 300, 11 + m_min_size / 300, 11 + m_min_size / 150);

    m_buttons.assign(new_board.rowSize(), std::vector<MineButton*>(new_board.colSize(), nullptr));
    for (int32_t i = 0; i < new_board.rowSize(); i++) {
        board_widget_layout->setRowMinimumHeight(i, 23);
        for (int32_t j = 0; j < new_board.colSize(); j++) {
            m_buttons[i][j] = new MineButton({ i, j }, board_widget);
            m_buttons[i][j]->setFixedSize(27, 27);
            m_buttons[i][j]->setFont(QFont(m_board_font, 14));
            m_buttons[i][j]->setIconSize(QSize(25, 25));

            connect(m_buttons[i][j], &MineButton::enableSurpriseFace, this, &GameWindow::onEnableSurpriseFace);
            connect(m_buttons[i][j], &MineButton::disableSurpriseFace, this, &GameWindow::onDisableSurpriseFace);
            connect(m_buttons[i][j], &MineButton::rmbReleased, this, &GameWindow::onMark);
            connect(m_buttons[i][j], &MineButton::lmbReleased, this, &GameWindow::onReveal);
            board_widget_layout->addWidget(m_buttons[i][j], i, j);
        }
    }

    updateBoard(new_board, { false, false }, true);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
    LOG_INFO("window: fixed size is {}, {}", size().width(), size().height());
}

void GameWindow::renderButton(const model::MineSquare& square, const model::GameState& new_state, MineButton* button) {
    const bool square_revealed = square.is_revealed;
    const bool square_marked = square.is_marked;
    const bool square_has_adj = square.adjacent_mines;
    const bool square_is_mine = square.is_mine;
    const bool square_is_end_reason = square.is_end_reason;

    QString prev_name = button->objectName();

    if (!square_revealed) {
        button->setObjectName("regular");
        button->setIcon(square_marked ? m_flag : m_no_icon);
        button->setText("");
        button->setDisabled(false);
        button->setClickableUi(new_state.lose || new_state.win || square_marked ? false : true);
    } else if (square_revealed) {
        if (square_is_mine) {
            button->setObjectName(square_is_end_reason ? "red_background" : "regular"); // For stylesheet
            button->setIcon(m_mine);
            button->setText("");
            button->setDisabled(true);
        } else if (square_has_adj) {
            // The font pack is messed up; The 74th character in the ascii table (which is
            // supposed to be an uppercase J) is the start of the numbers 0-9. Therefore,
            // we add s.adjacent_mines to 74 to convert it into the appropriate ascii
            // code for the font pack.
            char16_t temp[2] = { (char16_t) (74 + square.adjacent_mines), '\0' };
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

void GameWindow::onRestart() const
{
    emit restart();
}

void GameWindow::onReveal(const model::MineCoord& coord) const {
    emit reveal(coord);
}

void GameWindow::onMark(const model::MineCoord& coord) const {
    emit mark(coord);
}

void GameWindow::onClose() const {
    emit close();
}

void GameWindow::onMinimize() const {
    emit minimize();
}

void GameWindow::onEnableSurpriseFace() {
    if (m_prev_state.win || m_prev_state.lose)
        return;
    m_prev_state.revealing_mine = true;
    updateControlIcon(m_prev_state);
}

void GameWindow::onDisableSurpriseFace() {
    if (m_prev_state.win || m_prev_state.lose)
        return;
    m_prev_state.revealing_mine = false;
    updateControlIcon(m_prev_state);
}

void GameWindow::onActionBeginner() const {
    emit actionBeginner();
}

void GameWindow::onActionIntermediate() const {
    emit actionIntermediate();
}

void GameWindow::onActionAdvanced() const {
    emit actionAdvanced();
}

void GameWindow::onActionAbout() const {
    emit actionAbout();
}

void GameWindow::onActionCustom() const {
    emit actionCustom();
}

void GameWindow::onActionSetSafeFirstClick(bool safe) const {
    emit actionSetSafeFirstClick(safe);
}

void GameWindow::updateControlIcon(const model::GameState& state) {
    if (state.win) {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/win.png"));
    } else if (state.lose) {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/dead.png"));
    } else if (state.revealing_mine) {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/revealing.png"));
    } else {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/smile.png"));
    }
}

/********** Custom Title Bar Implementation **********/

void GameWindow::mousePressEvent(QMouseEvent* event) {
    QWidget* child = childAt(event->position().toPoint());
    if (child == window_bar || child == window_title) {
        m_prev_position = event->globalPosition();
        m_moving_window = true;
    }
}

void GameWindow::mouseMoveEvent(QMouseEvent* event) {
    if (m_moving_window) {
        const QPointF delta = event->globalPosition() - m_prev_position;
        move(x() + delta.x(), y() + delta.y());
        m_prev_position = event->globalPosition();
    }
}

void GameWindow::mouseReleaseEvent(QMouseEvent* event) {
    m_moving_window = false;
}

} // namespace view