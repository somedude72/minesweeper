#include "view/button.h"
#include <iostream>

namespace view {

    MineButton::MineButton(QWidget* parent) : QPushButton(parent) {
        connect(this, &QPushButton::clicked, this, &MineButton::on_clicked);
    }

    void MineButton::on_clicked() const {
        std::cout << "[button] clicked " << objectName().toStdString() << '\n';
        emit clicked(objectName());
    }

} // namespace view