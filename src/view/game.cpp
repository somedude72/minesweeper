#include "view/game.h"
#include "view/button.h"
#include "spdlog/spdlog.h"

#include "QString"
#include "QSizePolicy"

#include <cstdint>

namespace view {

    MineWindow::MineWindow(const model::MineBoard& init_state, QWidget* parent) : QMainWindow(parent) {
        setupUi(this);
        adjustSize();
        
        spdlog::debug("window size: {}, {}", size().width(), size().height());
        setMaximumHeight(size().height());
        setMaximumWidth(size().width());
        
        update_board(init_state);
        connect(m_ctrl_button_restart, &QPushButton::clicked, this, &MineWindow::on_restart);
    }
    
    void MineWindow::update_board(const model::MineBoard& new_state) {
        clear_board();
        int32_t row_size = new_state.row_size();
        int32_t col_size = new_state.col_size();

        m_grid_board->setSpacing(0);
        buttons.resize(row_size, std::vector<MineButton*>(col_size, nullptr));
        for (int32_t i = 0; i < row_size; i++) {
            for (int32_t j = 0; j < col_size; j++) {
                buttons[i][j] = new MineButton({ i, j }, this);
                buttons[i][j]->setFixedSize(30, 30);
                buttons[i][j]->setContentsMargins(0, 0, 0, 0);

                m_grid_board->addWidget(buttons[i][j], i, j);
                connect(buttons[i][j], &MineButton::right_clicked, this, &MineWindow::on_mark);
                connect(buttons[i][j], &MineButton::left_clicked, this, &MineWindow::on_reveal);

                model::MineSquare curr_square = new_state.get_square({ i, j });
                if (!curr_square.is_revealed) {
                    QString btn_text = curr_square.is_marked ? "🚩" : "";
                    buttons[i][j]->setDisabled(false);
                    buttons[i][j]->setText(btn_text);
                } else if (!curr_square.is_mine) {
                    buttons[i][j]->setDisabled(true);
                    buttons[i][j]->setText(QString::number(curr_square.adjacent_mines));
                } else {
                    buttons[i][j]->setDisabled(true);
                    buttons[i][j]->setText("💣");
                }
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