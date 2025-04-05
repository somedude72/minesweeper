#include "view/game.h"
#include "view/button.h"
#include "utils/config.h"

#include "QString"
#include "QFontDatabase"
#include "QIcon"
#include "QPixmap"

#include <cstdint>

namespace view {

MineWindow::MineWindow(const model::MineBoard& init_state, QWidget* parent) : QMainWindow(parent) {
    setupUi(this);
    adjustSize();
    
    LOG_INFO("window: initializing main ui");
    LOG_INFO("window: fixed size is {}, {}", size().width(), size().height());
    setMaximumHeight(size().height());
    setMaximumWidth(size().width());
    board_widget_layout->setVerticalSpacing(12);
    board_widget_layout->setHorizontalSpacing(0);

    ctrl_button_restart->setIconSize(QSize(27, 27));
    
    connect(ctrl_button_restart, &QPushButton::clicked, this, &MineWindow::on_restart);
    update_board(init_state, false, false);
}

void MineWindow::update_board(const model::MineBoard& new_state, bool lose, bool win) {
    clear_board();
    int32_t row_size = new_state.row_size();
    int32_t col_size = new_state.col_size();

    if (win) {
        ctrl_button_restart->setIcon(QIcon(":/assets/ms-win.png"));
    } else if (lose) {
        ctrl_button_restart->setIcon(QIcon(":/assets/ms-dead.png"));
    } else {
        ctrl_button_restart->setIcon(QIcon(":/assets/ms-smile.png"));
    }
    
    buttons.resize(row_size, std::vector<MineButton*>(col_size, nullptr));
    for (int32_t i = 0; i < row_size; i++) {
        for (int32_t j = 0; j < col_size; j++) {
            buttons[i][j] = new MineButton({ i, j }, this);
            buttons[i][j]->setFixedSize(30, 30);
            buttons[i][j]->setContentsMargins(0, 0, 0, 0);
            connect(buttons[i][j], &MineButton::right_clicked, this, &MineWindow::on_mark);
            connect(buttons[i][j], &MineButton::left_clicked, this, &MineWindow::on_reveal);
            board_widget_layout->addWidget(buttons[i][j], i, j);

            const model::MineSquare& curr_square = new_state.get_square({ i, j });
            render_button(curr_square, buttons[i][j], lose, win);
        }
    }
}

void MineWindow::render_button(const model::MineSquare& s, MineButton* button, bool lose, bool win) {
    QIcon flag, mine, redx, none;
    flag.addPixmap(QPixmap(":/assets/ms-flag.png"), QIcon::Disabled);
    mine.addPixmap(QPixmap(":/assets/ms-mine.png"), QIcon::Disabled);
    redx.addPixmap(QPixmap(":/assets/ms-cross.png"), QIcon::Disabled);
    flag.addPixmap(QPixmap(":/assets/ms-flag.png"), QIcon::Normal);
    mine.addPixmap(QPixmap(":/assets/ms-mine.png"), QIcon::Normal);
    redx.addPixmap(QPixmap(":/assets/ms-cross.png"), QIcon::Normal);


    // From https://stackoverflow.com/questions/30973781/qt-add-custom-font-from-resource
    int id = QFontDatabase::addApplicationFont(":/assets/ms-numbers.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    button->setFont(QFont(family, 16));
    button->setIconSize(QSize(27, 27));

    const bool is_reveal = s.is_revealed;
    const bool is_mark = s.is_marked;
    const bool is_adj = s.adjacent_mines;
    const bool is_mine = s.is_mine;
    const bool is_end_reason = s.is_end_reason;

    if (!is_reveal) {
        button->setObjectName(lose || is_mark ? "unclickable" : "regular");
        button->setIcon(is_mark ? flag : none);
        button->setDisabled(false);
    } else {
        if (is_mine) {
            button->setObjectName(is_end_reason ? "red_background" : "unclickable"); // For stylesheet
            button->setIcon(is_mark ? redx : mine);
            button->setDisabled(true);
        } else if (is_adj) {
            // The font pack is messed up; The 74th character in the ascii table (which is
            // supposed to be an uppercase J) is the start of the numbers 0-9. Therefore,
            // we add s.adjacent_mines to 74 to convert it into the appropriate ascii
            // code for the font pack.
            char16_t c[2] = { (char16_t) (74 + s.adjacent_mines), '\0' };
            QString temp = QString::fromUtf16(c);
            button->setObjectName("mine_" + QString::number(s.adjacent_mines)); // For stylesheet
            button->setText(temp);
            button->setDisabled(true);
        } else {
            button->setObjectName(lose ? "unclickable" : "regular");
            button->setDisabled(true);
        }
    }
}

void MineWindow::clear_board() {
    for (int32_t i = 0; i < buttons.size(); i++) {
        for (int32_t j = 0; j < buttons[0].size(); j++) {
            board_widget_layout->removeWidget(buttons[i][j]);
            buttons[i][j]->deleteLater();
            buttons[i][j] = nullptr;
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

} // namespace view