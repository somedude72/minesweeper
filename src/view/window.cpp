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
    adjustSize();
    
    LOG_INFO("window: initializing main ui");
    LOG_INFO("window: fixed size is {}, {}", size().width(), size().height());
    setMaximumSize(size().width(), size().height());

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

    int32_t row_size = init_board.row_size();
    int32_t col_size = init_board.col_size();
    m_buttons.resize(row_size, std::vector<MineButton*>(col_size));
    for (int32_t i = 0; i < row_size; i++) {
        for (int32_t j = 0; j < col_size; j++) {
            m_buttons[i][j] = new MineButton({ i, j }, board_widget);
            m_buttons[i][j]->setFixedSize(30, 30);
            m_buttons[i][j]->setContentsMargins(0, 0, 0, 0);
            m_buttons[i][j]->setFont(QFont(number_font, 16));
            m_buttons[i][j]->setIconSize(QSize(27, 27));

            connect(m_buttons[i][j], &MineButton::enable_surprise_face, this, &MineWindow::on_enable_surprise_face);
            connect(m_buttons[i][j], &MineButton::disable_surprise_face, this, &MineWindow::on_disable_surprise_face);
            connect(m_buttons[i][j], &MineButton::rmb_released, this, &MineWindow::on_mark);
            connect(m_buttons[i][j], &MineButton::lmb_released, this, &MineWindow::on_reveal);
            board_widget_layout->addWidget(m_buttons[i][j], i, j);
        }
    }

    m_flag.addPixmap(QPixmap(":/assets/board/flag.png"), QIcon::Disabled);
    m_flag.addPixmap(QPixmap(":/assets/board/flag.png"), QIcon::Normal);
    m_mine.addPixmap(QPixmap(":/assets/board/mine.png"), QIcon::Disabled);
    m_mine.addPixmap(QPixmap(":/assets/board/mine.png"), QIcon::Normal);
    m_marked_mine.addPixmap(QPixmap(":/assets/board/cross.png"), QIcon::Disabled);
    m_marked_mine.addPixmap(QPixmap(":/assets/board/cross.png"), QIcon::Normal);

    connect(window_close, &QPushButton::clicked, this, &MineWindow::on_close);
    connect(window_min, &QPushButton::clicked, this, &MineWindow::on_minimize);
    connect(ctrl_button_restart, &QPushButton::clicked, this, &MineWindow::on_restart);
    update_window(init_board, { false, false }, true);
}

void MineWindow::update_window(const model::MineBoard& new_board, const model::GameState& new_state, bool first_render) {
    update_control_icon(new_state);

    int32_t row_size = new_board.row_size();
    int32_t col_size = new_board.col_size();
    for (int32_t i = 0; i < row_size; i++) {
        for (int32_t j = 0; j < col_size; j++) {
            // Due to the performance overhead of updating/repainting widgets with
            // stylesheets, we should only update mine squares that have been updated.
            if (first_render || m_prev_state != new_state || new_board.get_square({ i, j }) != m_prev_board.get_square({ i, j })) {
                render_button(new_board.get_square({ i, j }), new_state, m_buttons[i][j]);
            }
        }
    }
    
    m_prev_state = new_state;
    m_prev_board = new_board;
    adjustSize();
}

void MineWindow::render_button(const model::MineSquare& square, const model::GameState& new_state, MineButton* button) {
    const bool square_revealed = square.is_revealed;
    const bool square_marked = square.is_marked;
    const bool square_has_adj = square.adjacent_mines;
    const bool square_is_mine = square.is_mine;
    const bool square_is_end_reason = square.is_end_reason;

    if (!square_revealed) {
        button->setObjectName(new_state.lose || new_state.win || square_marked ? "unclickable" : "regular"); // For stylesheet
        button->setIcon(square_marked ? m_flag : m_no_icon);
        button->setText("");
        button->setDisabled(false);
    } else { // Revealed square
        if (square_is_mine) {
            button->setObjectName(square_is_end_reason ? "red_background" : "unclickable"); // For stylesheet
            button->setIcon(square_marked ? m_marked_mine : m_mine);
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
            button->setIcon(m_no_icon);
            button->setText("");
            button->setDisabled(true);
        }
    }

    button->style()->polish(button);
}

void MineWindow::on_restart() const {
    emit restart();
}

void MineWindow::on_reveal(const model::MineCoord& coord) const {
    emit reveal(coord);
}

void MineWindow::on_mark(const model::MineCoord& coord) const {
    emit mark(coord);
}

void MineWindow::on_close() const {
    emit close();
}

void MineWindow::on_minimize() const {
    emit minimize();
}

void MineWindow::on_enable_surprise_face() {
    if (m_prev_state.win || m_prev_state.lose)
        return;
    m_prev_state.revealing_mine = true;
    update_control_icon(m_prev_state);
}

void MineWindow::on_disable_surprise_face() {
    if (m_prev_state.win || m_prev_state.lose)
        return;
    m_prev_state.revealing_mine = false;
    update_control_icon(m_prev_state);
}

void MineWindow::update_control_icon(const model::GameState& state) {
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
    QWidget* child = childAt(event->position());
    if (window_bar == child || window_title == child) {
        m_prev_position = event->globalPosition();
        m_moving_window = true;
    }
}

void MineWindow::mouseMoveEvent(QMouseEvent* event) {
    QWidget* child = childAt(event->position());
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