#include "view/button.h"
#include "model/screen.h"
#include "model/data.h"
#include "fmt/format.h"

#include "QPushButton"
#include "QMouseEvent"

static const std::string button_style = R"(

/* Regular button styles (non end game) */
QPushButton#regular {{
    border: 0px solid gray;
    border-right: {0}px solid gray;
    border-bottom: {0}px solid gray;
    border-top: {0}px solid white;
    border-left: {0}px solid white;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}
    
QPushButton#regular:pressed {{
    border: 0px solid gray;
    border-right: {0}px solid white;
    border-bottom: {0}px solid white;
    border-top: {0}px solid gray;
    border-left: {0}px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}

QPushButton#regular:disabled {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}

QPushButton#unclickable:disabled {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}

QPushButton#red_background:disabled {{
    border: 1px solid gray;
    background-color: rgb(255, 0, 0);
    color: rgb(0, 0, 0);
}}

/* Buttons with numbers */
QPushButton#mine_1 {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(32, 32, 245);
}}

QPushButton#mine_2 {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 128, 0);
}}

QPushButton#mine_3 {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(255, 0, 0);
}}

QPushButton#mine_4 {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 128);
}}

QPushButton#mine_5 {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(128, 0, 0);
}}

QPushButton#mine_6 {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 128, 128);
}}

QPushButton#mine_7 {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}}

QPushButton#mine_8 {{
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(128, 128, 128);
}}
)";

namespace view {

MineButton::MineButton(const model::MineCoord& coord, QWidget* parent) : QPushButton(parent) {
    m_clickable = true;
    m_coord = coord;

    const int32_t border_size = model::Screen::getMinSize() / 300;
    setStyleSheet(QString::fromStdString(fmt::format(button_style, border_size)));
}

void MineButton::setClickableUi(bool clickable) {
    m_clickable = clickable;
}

void MineButton::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton)
        emit enableSurpriseFace();
    if (m_clickable)
        QPushButton::mousePressEvent(event);
}

void MineButton::mouseReleaseEvent(QMouseEvent* event) {
    emit disableSurpriseFace();

    if (!rect().contains(event->pos()))
        return;
    if (event->button() == Qt::RightButton)
        emit rmbReleased(m_coord);
    if (event->button() == Qt::LeftButton) {
        emit lmbReleased(m_coord);
    }

    QPushButton::mouseReleaseEvent(event);
}

} // namespace view