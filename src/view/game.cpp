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
    setMaximumSize(size().width(), size().height());
    board_widget_layout->setVerticalSpacing(12);
    board_widget_layout->setHorizontalSpacing(0);

    ctrl_button_restart->setIconSize(QSize(27, 27));
    
    connect(ctrl_button_restart, &QPushButton::clicked, this, &MineWindow::on_restart);
    render_board(init_state, false, false);
}

void MineWindow::render_board(const model::MineBoard& new_state, bool lose, bool win) {
    adjustSize();
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

            render_button(new_state.get_square({ i, j }), buttons[i][j], lose, win);
        }
    }
}

void MineWindow::render_button(const model::MineSquare& s, MineButton* button, bool lose, bool win) {
    QIcon flag, mine, marked_mine, none;
    flag.addPixmap(QPixmap(":/assets/ms-flag.png"), QIcon::Disabled);
    flag.addPixmap(QPixmap(":/assets/ms-flag.png"), QIcon::Normal);
    mine.addPixmap(QPixmap(":/assets/ms-mine.png"), QIcon::Disabled);
    mine.addPixmap(QPixmap(":/assets/ms-mine.png"), QIcon::Normal);
    marked_mine.addPixmap(QPixmap(":/assets/ms-cross.png"), QIcon::Disabled);
    marked_mine.addPixmap(QPixmap(":/assets/ms-cross.png"), QIcon::Normal);


    // From https://stackoverflow.com/questions/30973781/qt-add-custom-font-from-resource
    int id = QFontDatabase::addApplicationFont(":/assets/ms-numbers.ttf");
    QString family = QFontDatabase::applicationFontFamilies(id).at(0);
    button->setFont(QFont(family, 16));
    button->setIconSize(QSize(27, 27));

    const bool square_revealed = s.is_revealed;
    const bool square_marked = s.is_marked;
    const bool square_has_adj = s.adjacent_mines;
    const bool square_is_mine = s.is_mine;
    const bool square_is_end_reason = s.is_end_reason;

    if (!square_revealed) {
        button->setObjectName(lose || square_marked || win ? "unclickable" : "regular"); // For stylesheet
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