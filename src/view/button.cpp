#include "view/button.h"
#include "model/data.h"

#include <QPushButton>
#include <QMouseEvent>

namespace view {

    MineButton::MineButton(const model::MineCoord& coord, QWidget* parent) : QPushButton(parent) {
        m_coord = coord;
    }

    void MineButton::mouseReleaseEvent(QMouseEvent* event) {
        if (event->button() == Qt::RightButton)
            emit right_clicked(m_coord);
        if (event->button() == Qt::LeftButton)
            emit left_clicked(m_coord);
    }

} // namespace view