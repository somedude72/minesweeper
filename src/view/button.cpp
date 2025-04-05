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

/* End game button styles (static) */
QPushButton#end {
    border: 0px solid gray;
    border-right: 4px solid gray;
    border-bottom: 4px solid gray;
    border-top: 4px solid white;
    border-left: 4px solid white;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}

QPushButton#end:pressed {
    border: 0px solid gray;
    border-right: 4px solid gray;
    border-bottom: 4px solid gray;
    border-top: 4px solid white;
    border-left: 4px solid white;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}

QPushButton#end:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}

QPushButton#end_bomb:disabled {
    border: 1px solid gray;
    background-color: rgb(255, 0, 0);
    color: rgb(0, 0, 0);
}

/* Buttons with numbers */
QPushButton#mine_1:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(32, 32, 245);
}

QPushButton#mine_2:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 128, 0);
}

QPushButton#mine_3:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(255, 0, 0);
}

QPushButton#mine_4:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 128);
}

QPushButton#mine_5:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(128, 0, 0);
}

QPushButton#mine_6:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 128, 128);
}

QPushButton#mine_7:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(0, 0, 0);
}

QPushButton#mine_8:disabled {
    border: 1px solid gray;
    background-color: rgb(205, 205, 205);
    color: rgb(128, 128, 128);
}
)";

namespace view {

MineButton::MineButton(const model::MineCoord& coord, QWidget* parent) : QPushButton(parent) {
    setStyleSheet(button_style);
    m_coord = coord;
}

void MineButton::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::RightButton)
        emit right_clicked(m_coord);
    if (event->button() == Qt::LeftButton)
        emit left_clicked(m_coord);
}

} // namespace view