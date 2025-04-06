#include "view/window.h"
#include "model/data.h"
#include "view/button.h"
#include "utils/config.h"

#include "QString"
#include "QPoint"
#include "QMouseEvent"

#include "QFontDatabase"
#include "QIcon"
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
    int id = QFontDatabase::addApplicationFont(":/assets/window/font.otf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    window_close->setIcon(QIcon(":/assets/window/close.png"));
    window_min->setIcon(QIcon(":/assets/window/minimize.png"));
    window_title->setText("Minesweeper");
    
    window_close->setIconSize(QSize(15, 15));
    window_min->setIconSize(QSize(15, 15));
    ctrl_button_restart->setIconSize(QSize(27, 27));
    
    window_title->setFont(QFont(family, 18));
    menu_game->setFont(QFont(family, 15, 800));
    menu_help->setFont(QFont(family, 15, 800));

    connect(window_close, &QPushButton::clicked, this, &MineWindow::on_close);
    connect(window_min, &QPushButton::clicked, this, &MineWindow::on_minimize);
    connect(ctrl_button_restart, &QPushButton::clicked, this, &MineWindow::on_restart);
    update_window(init_board, { false, false });
}

void MineWindow::update_window(const model::MineBoard& new_board, const model::GameState& state) {
    clear_board();
    int32_t row_size = new_board.row_size();
    int32_t col_size = new_board.col_size();
    m_state = state;
    update_icon();
    
    m_buttons.resize(row_size, std::vector<MineButton*>(col_size, nullptr));
    for (int32_t i = 0; i < row_size; i++) {
        for (int32_t j = 0; j < col_size; j++) {
            m_buttons[i][j] = new MineButton({ i, j }, this);
            m_buttons[i][j]->setFixedSize(30, 30);
            m_buttons[i][j]->setContentsMargins(0, 0, 0, 0);
            connect(m_buttons[i][j], &MineButton::enable_surprise_face, this, &MineWindow::on_enable_surprise_face);
            connect(m_buttons[i][j], &MineButton::disable_surprise_face, this, &MineWindow::on_disable_surprise_face);
            connect(m_buttons[i][j], &MineButton::rmb_released, this, &MineWindow::on_mark);
            connect(m_buttons[i][j], &MineButton::lmb_released, this, &MineWindow::on_reveal);
            board_widget_layout->addWidget(m_buttons[i][j], i, j);
            
            render_button(new_board.get_square({ i, j }), m_buttons[i][j]);
        }
    }

    adjustSize();
}

void MineWindow::render_button(const model::MineSquare& s, MineButton* button) {
    QIcon flag, mine, marked_mine, none;
    flag.addPixmap(QPixmap(":/assets/board/flag.png"), QIcon::Disabled);
    flag.addPixmap(QPixmap(":/assets/board/flag.png"), QIcon::Normal);
    mine.addPixmap(QPixmap(":/assets/board/mine.png"), QIcon::Disabled);
    mine.addPixmap(QPixmap(":/assets/board/mine.png"), QIcon::Normal);
    marked_mine.addPixmap(QPixmap(":/assets/board/cross.png"), QIcon::Disabled);
    marked_mine.addPixmap(QPixmap(":/assets/board/cross.png"), QIcon::Normal);

    // From https://stackoverflow.com/questions/30973781/qt-add-custom-font-from-resource
    int id = QFontDatabase::addApplicationFont(":/assets/board/font.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);

    button->setFont(QFont(family, 16));
    button->setIconSize(QSize(27, 27));

    const bool square_revealed = s.is_revealed;
    const bool square_marked = s.is_marked;
    const bool square_has_adj = s.adjacent_mines;
    const bool square_is_mine = s.is_mine;
    const bool square_is_end_reason = s.is_end_reason;

    if (!square_revealed) {
        button->setObjectName(m_state.lose || m_state.win || square_marked ? "unclickable" : "regular"); // For stylesheet
        button->setIcon(square_marked ? flag : none);
        button->setDisabled(false);
    } else { // Revealed square
        if (square_is_mine) {
            button->setObjectName(square_is_end_reason ? "red_background" : "unclickable"); // For stylesheet
            button->setIcon(square_marked ? marked_mine : mine);
            button->setDisabled(true);
        } else if (square_has_adj) {
            // The font pack is messed up; The 74th character in the ascii table (which is
            // supposed to be an uppercase J) is the start of the numbers 0-9. Therefore,
            // we add s.adjacent_mines to 74 to convert it into the appropriate ascii
            // code for the font pack.
            char16_t temp[2] = { (char16_t) (74 + s.adjacent_mines), '\0' };
            button->setObjectName("mine_" + QString::number(s.adjacent_mines)); // For stylesheet
            button->setText(QString::fromUtf16(temp));
            button->setDisabled(true);
        } else {
            button->setObjectName("regular");
            button->setDisabled(true);
        }
    }
}

void MineWindow::clear_board() {
    for (int32_t i = 0; i < m_buttons.size(); i++) {
        for (int32_t j = 0; j < m_buttons[0].size(); j++) {
            board_widget_layout->removeWidget(m_buttons[i][j]);
            m_buttons[i][j]->deleteLater();
            m_buttons[i][j] = nullptr;
        }
    }
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
    if (m_state.win || m_state.lose)
        return;
    m_state.revealing_mine = true;
    update_icon();
}

void MineWindow::on_disable_surprise_face() {
    if (m_state.win || m_state.lose)
        return;
    m_state.revealing_mine = false;
    update_icon();
}

void MineWindow::update_icon() {
    if (m_state.win) {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/win.png"));
    } else if (m_state.lose) {
        ctrl_button_restart->setIcon(QIcon(":/assets/board/dead.png"));
    } else if (m_state.revealing_mine) {
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