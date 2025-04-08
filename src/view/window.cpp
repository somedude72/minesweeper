#include "view/window.h"
#include "model/data.h"
#include "view/button.h"
#include "utils/config.h"

#include "QString"
#include "QPoint"
#include "QMouseEvent"

#include "QFontDatabase"
#include "QIcon"
#include "QStyle"
#include "QPixmap"

#include <cstdint>

namespace view {

MineWindow::MineWindow(const model::MineBoard& init_board, QWidget* parent) : QMainWindow(parent) {
    setupUi(this);
    
    LOG_INFO("window: initializing main ui");

    // From https://stackoverflow.com/questions/30973781/qt-add-custom-font-from-resource
    int window_font_id = QFontDatabase::addApplicationFont(":/assets/window/font.otf");
    int board_font_id = QFontDatabase::addApplicationFont(":/assets/board/font.ttf");
    QString window_font = QFontDatabase::applicationFontFamilies(window_font_id).at(0);
    QString number_font = QFontDatabase::applicationFontFamilies(board_font_id).at(0);

    window_close->setIcon(QIcon(":/assets/window/close.png"));
    window_min->setIcon(QIcon(":/assets/window/minimize.png"));
    window_title->setFont(QFont(window_font, 18));
    
    menu_game->setFont(QFont(window_font, 15, 800));
    menu_help->setFont(QFont(window_font, 15, 800));

    int32_t row_size = init_board.rowSize();
    int32_t col_size = init_board.colSize();
    m_buttons.resize(row_size, std::vector<MineButton*>(col_size));
    for (int32_t i = 0; i < row_size; i++) {
        board_widget_layout->setRowMinimumHeight(i, 26);
        board_widget_layout->setColumnMinimumWidth(i, 30);
        for (int32_t j = 0; j < col_size; j++) {
            m_buttons[i][j] = new MineButton({ i, j }, board_widget);
            m_buttons[i][j]->setFixedSize(30, 30);
            m_buttons[i][j]->setContentsMargins(0, 0, 0, 0);
            m_buttons[i][j]->setFont(QFont(number_font, 16));
            m_buttons[i][j]->setIconSize(QSize(27, 27));

            connect(m_buttons[i][j], &MineButton::enableSurpriseFace, this, &MineWindow::onEnableSurpriseFace);
            connect(m_buttons[i][j], &MineButton::disableSurpriseFace, this, &MineWindow::onDisableSurpriseFace);
            connect(m_buttons[i][j], &MineButton::rmbReleased, this, &MineWindow::onMark);
            connect(m_buttons[i][j], &MineButton::lmbReleased, this, &MineWindow::onReveal);
            board_widget_layout->addWidget(m_buttons[i][j], i, j);
        }
    }

    m_flag.addPixmap(QPixmap(":/assets/board/flag.png"), QIcon::Disabled);
    m_flag.addPixmap(QPixmap(":/assets/board/flag.png"), QIcon::Normal);
    m_mine.addPixmap(QPixmap(":/assets/board/mine.png"), QIcon::Disabled);
    m_mine.addPixmap(QPixmap(":/assets/board/mine.png"), QIcon::Normal);
    m_wrong_mine.addPixmap(QPixmap(":/assets/board/cross.png"), QIcon::Disabled);
    m_wrong_mine.addPixmap(QPixmap(":/assets/board/cross.png"), QIcon::Normal);

    connect(window_close, &QPushButton::clicked, this, &MineWindow::onClose);
    connect(window_min, &QPushButton::clicked, this, &MineWindow::onMinimize);
    connect(ctrl_button_restart, &QPushButton::clicked, this, &MineWindow::onRestart);
    updateWindow(init_board, { false, false }, true);

    setFixedSize(size());
    LOG_INFO("window: fixed size is {}, {}", size().width(), size().height());
}

void MineWindow::updateWindow(const model::MineBoard& new_board, const model::GameState& new_state, bool first_render) {
    updateControlIcon(new_state);

    int32_t row_size = new_board.rowSize();
    int32_t col_size = new_board.colSize();
    for (int32_t i = 0; i < row_size; i++) {
        for (int32_t j = 0; j < col_size; j++) {
            // Due to the performance overhead of updating/repainting widgets with
            // stylesheets, we should only update mine squares that have been updated.
            if (first_render || m_prev_state != new_state || new_board.getSquare({ i, j }) != m_prev_board.getSquare({ i, j })) {
                renderButton(new_board.getSquare({ i, j }), new_state, m_buttons[i][j]);
            }
        }
    }
    
    m_prev_state = new_state;
    m_prev_board = new_board;
    adjustSize();
}

void MineWindow::renderButton(const model::MineSquare& square, const model::GameState& new_state, MineButton* button) {
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
            button->setIcon(m_no_icon);
            button->setText(QString::fromUtf16(temp));
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

void MineWindow::onRestart() const {
    emit restart();
}

void MineWindow::onReveal(const model::MineCoord& coord) const {
    emit reveal(coord);
}

void MineWindow::onMark(const model::MineCoord& coord) const {
    emit mark(coord);
}

void MineWindow::onClose() const {
    emit close();
}

void MineWindow::onMinimize() const {
    emit minimize();
}

void MineWindow::onEnableSurpriseFace() {
    if (m_prev_state.win || m_prev_state.lose)
        return;
    m_prev_state.revealing_mine = true;
    updateControlIcon(m_prev_state);
}

void MineWindow::onDisableSurpriseFace() {
    if (m_prev_state.win || m_prev_state.lose)
        return;
    m_prev_state.revealing_mine = false;
    updateControlIcon(m_prev_state);
}

void MineWindow::updateControlIcon(const model::GameState& state) {
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

void MineWindow::mousePressEvent(QMouseEvent* event) {
    QWidget* child = childAt(event->position().toPoint());
    if (window_bar == child || window_title == child) {
        m_prev_position = event->globalPosition();
        m_moving_window = true;
    }
}

void MineWindow::mouseMoveEvent(QMouseEvent* event) {
    if (m_moving_window) {
        const QPointF delta = event->globalPosition() - m_prev_position;
        move(x() + delta.x(), y() + delta.y());
        m_prev_position = event->globalPosition();
    }
}

void MineWindow::mouseReleaseEvent(QMouseEvent* event) {
    m_moving_window = false;
}

} // namespace view