#include "view/button.h"
#include "model/data.h"

#include "QPushButton"
#include "QMouseEvent"

namespace view {

    const char* btn_stylesheet = R"(
        QPushButton {
            border: 0px solid gray;
            border-right: 4px solid gray;
            border-bottom: 4px solid gray;
            border-top: 4px solid white;
            border-left: 4px solid white;
            background-color: rgb(205, 205, 205);
            color: rgb(0, 0, 0);
        }
            
        QPushButton:pressed {
            border: 0px solid gray;
            border-right: 4px solid white;
            border-bottom: 4px solid white;
            border-top: 4px solid gray;
            border-left: 4px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(0, 0, 0);
        }
            
        QPushButton:disabled {
            border: 1px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(0, 0, 0);
        }

        QPushButton#1m:disabled {
            border: 1px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(32, 32, 245);
        }

        QPushButton#2m:disabled {
            border: 1px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(0, 128, 0);
        }

        QPushButton#3m:disabled {
            border: 1px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(255, 0, 0);
        }

        QPushButton#4m:disabled {
            border: 1px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(0, 0, 128);
        }

        QPushButton#5m:disabled {
            border: 1px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(128, 0, 0);
        }

        QPushButton#6m:disabled {
            border: 1px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(0, 128, 128);
        }

        QPushButton#7m:disabled {
            border: 1px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(0, 0, 0);
        }

        QPushButton#8m:disabled {
            border: 1px solid gray;
            background-color: rgb(205, 205, 205);
            color: rgb(128, 128, 128);
        }

        QPushButton#red:disabled {
            border: 1px solid gray;
            background-color: rgb(255, 0, 0);
            color: rgb(0, 0, 0);
        }
    )";

    MineButton::MineButton(const model::MineCoord& coord, QWidget* parent) : QPushButton(parent) {
        setStyleSheet(btn_stylesheet);
        m_coord = coord;
    }

    void MineButton::mouseReleaseEvent(QMouseEvent* event) {
        if (event->button() == Qt::RightButton)
            emit right_clicked(m_coord);
        if (event->button() == Qt::LeftButton)
            emit left_clicked(m_coord);
    }

} // namespace view