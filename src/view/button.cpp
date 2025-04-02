#include "view/button.h"
#include "model/data.h"

namespace view {

    MineButton::MineButton(const model::MineCoord& coord, QWidget* parent) : QPushButton(parent) {
        connect(this, &QPushButton::clicked, this, &MineButton::on_clicked);
        m_coord = coord;
    }

    void MineButton::on_clicked() const {
        emit clicked(m_coord);
    }

} // namespace view