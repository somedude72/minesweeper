#include "view/game.h"
#include "view/button.h"
#include "config.h"

#include "QString"
#include "QSizePolicy"
#include "QFontDatabase"
#include "QFile"
#include "QStyle"

#include <cstdint>

namespace view {

    MineWindow::MineWindow(const model::MineBoard& init_state, QWidget* parent) : QMainWindow(parent) {
        setupUi(this);
        adjustSize();
        
        LOG_INFO("window: initializing main ui");
        LOG_INFO("window: fixed size is {}, {}", size().width(), size().height());
        setMaximumHeight(size().height());
        setMaximumWidth(size().width());
        
        update_board(init_state);
        m_grid_board->setSpacing(0);

        connect(m_ctrl_button_restart, &QPushButton::clicked, this, &MineWindow::on_restart);
    }
    
    void MineWindow::update_board(const model::MineBoard& new_state) {
        clear_board();
        int32_t row_size = new_state.row_size();
        int32_t col_size = new_state.col_size();
        
        buttons.resize(row_size, std::vector<MineButton*>(col_size, nullptr));
        for (int32_t i = 0; i < row_size; i++) {
            for (int32_t j = 0; j < col_size; j++) {
                buttons[i][j] = new MineButton({ i, j }, this);
                buttons[i][j]->setFixedSize(30, 30);
                buttons[i][j]->setContentsMargins(0, 0, 0, 0);

                m_grid_board->addWidget(buttons[i][j], i, j);
                connect(buttons[i][j], &MineButton::right_clicked, this, &MineWindow::on_mark);
                connect(buttons[i][j], &MineButton::left_clicked, this, &MineWindow::on_reveal);

                const model::MineSquare& curr_square = new_state.get_square({ i, j });
                render_button(curr_square, buttons[i][j]);
            }
        }
    }

    void MineWindow::render_button(const model::MineSquare& s, MineButton* button) {
        const QIcon flag(":/assets/ms-flag.png");
        const QIcon mine(":/assets/ms-mine.png");
        const QIcon none;

        // From https://stackoverflow.com/questions/30973781/qt-add-custom-font-from-resource
        int id = QFontDatabase::addApplicationFont(":/assets/ms-numbers.ttf");
        QString family = QFontDatabase::applicationFontFamilies(id).at(0);
        button->setFont(QFont(family, 16));
        button->setIconSize(QSize(27, 27));

        const bool is_reveal = s.is_revealed;
        const bool is_mark = s.is_marked;
        const bool is_adj = s.adjacent_mines;
        const bool is_mine = s.is_mine;
        const bool is_end = s.is_reason_for_end;

        if (!is_reveal) {
            button->setText("");
            button->setIcon(is_mark ? flag : none);
            button->setDisabled(false);
        } else {
            if (is_mine) {
                button->setObjectName(is_end ? "red" : ""); // For stylesheet
                button->setText("!");
                button->setIcon(none);
                button->setDisabled(true);
            } else if (is_adj) {
                // The font pack is messed up; The 74th character in the ascii table (which is
                // supposed to be an uppercase J) is the start of the numbers 0-9. Therefore,
                // we add s.adjacent_mines to 74 to convert it into the appropriate ascii
                // code.
                char16_t c[2] = { (char16_t) (74 + s.adjacent_mines), '\0' };
                QString temp = QString::fromUtf16(c);
                button->setObjectName(QString::number(s.adjacent_mines) + "m"); // For stylesheet
                button->setText(temp);
                button->setIcon(none);
                button->setDisabled(true);
            } else {
                button->setText("");
                button->setIcon(none);
                button->setDisabled(true);
            }
        }
    }

    void MineWindow::clear_board() {
        for (int32_t i = 0; i < buttons.size(); i++) {
            for (int32_t j = 0; j < buttons[0].size(); j++) {
                m_grid_board->removeWidget(buttons[i][j]);
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