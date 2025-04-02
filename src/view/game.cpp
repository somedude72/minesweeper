#include "view/game.h"
#include "view/button.h"

#include <QString>
#include <QFont>
#include <cstdint>
#include <iostream>

namespace view {

    MineWindow::MineWindow(const model::MineBoard& init_state, QWidget* parent)
        : QMainWindow(parent) {

        setupUi(this);

        for (int32_t i = 0; i < init_state.row_size(); i++) {
            for (int32_t j = 0; j < init_state.col_size(); j++) {
                MineButton* btn = new MineButton(this);
                btn->setObjectName(QString::number(i) + QString::number(j));
                btn->setFont(QFont("Helvetica", 12));
                connect(btn, &MineButton::clicked, this, &MineWindow::on_reveal);
                vbox_board->addWidget(btn, i, j);
            }
        }

        connect(ctrl_button_restart, &QPushButton::clicked, this, &MineWindow::on_restart);
        update(init_state);
    }

    void MineWindow::update(const model::MineBoard& new_state) {
        for (int32_t i = 0; i < new_state.row_size(); i++) {
            for (int32_t j = 0; j < new_state.col_size(); j++) {
                QString btn_id = QString::number(i) + QString::number(j);
                MineButton* curr_btn = this->findChild<MineButton*>(btn_id);
                model::MineSquare curr_square = new_state.get_square({ i, j });

                if (!curr_square.is_revealed) {
                    curr_btn->setDisabled(false);
                    curr_btn->setText("");
                } else if (!curr_square.is_mine) {
                    curr_btn->setDisabled(true);
                    curr_btn->setText(QString::number(curr_square.adjacent_mines));
                } else {
                    curr_btn->setDisabled(true);
                    curr_btn->setText("💣");
                }
            }
        }
    }

    void MineWindow::on_restart() const {
        std::cout << "restarting" << '\n';
        emit restart();
    }

    void MineWindow::on_reveal(const QString& id) const {
        std::cout << "[game] revealing " << id.toStdString() << '\n';
        int32_t row = id[0].digitValue();
        int32_t col = id[1].digitValue();
        emit reveal({ row, col });
    }

} // namespace view