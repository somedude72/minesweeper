#include "view/button.h"
#include "model/data.h"

#include "QPushButton"
#include "QMouseEvent"

static const char* button_style = R"(

/* Regular button styles (non end game) */
QPushButton#regular {
    border: 0px solid gray;
    border-right: 4px solid gray;
    border-bottom: 4px solid gray;
    border-top: 4px solid white;
    border-left: 4px solid white;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}
    
QPushButton#regular:pressed {
    border: 0px solid gray;
    border-right: 4px solid white;
    border-bottom: 4px solid white;
    border-top: 4px solid gray;
    border-left: 4px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}

QPushButton#regular:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}

QPushButton#unclickable:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}

QPushButton#red_background:disabled {
    border: 1px solid gray;
    background-color: rgb(255, 0, 0);
    color: rgb(0, 0, 0);
}

/* Buttons with numbers */
QPushButton#mine_1 {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(32, 32, 245);
}

QPushButton#mine_2 {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 128, 0);
}

QPushButton#mine_3 {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(255, 0, 0);
}

QPushButton#mine_4 {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 128);
}

QPushButton#mine_5 {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(128, 0, 0);
}

QPushButton#mine_6 {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 128, 128);
}

QPushButton#mine_7 {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}

QPushButton#mine_8 {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(128, 128, 128);
}
)";

namespace view {

MineButton::MineButton(const model::MineCoord& coord, QWidget* parent) : QPushButton(parent) {
    setStyleSheet(button_style);
    m_clickable = true;
    m_coord = coord;
}

void MineButton::set_clickable_ui(bool clickable) {
    m_clickable = clickable;
}

void MineButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton)
        emit enable_surprise_face();
    if (m_clickable)
        QPushButton::mousePressEvent(event);
}

void MineButton::mouseReleaseEvent(QMouseEvent* event) {
    emit disable_surprise_face();

    if (!rect().contains(event->pos()))
        return;
    if (event->button() == Qt::RightButton)
        emit rmb_released(m_coord);
    if (event->button() == Qt::LeftButton) {
        emit lmb_released(m_coord);
    }

    if (m_clickable)
        QPushButton::mouseReleaseEvent(event);
}

} // namespace view