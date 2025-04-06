#pragma once

#include "model/data.h"

#include "QPushButton"
#include "QMouseEvent"
#include "QPoint"
#include "QWidget"

namespace view {

// Wrapper class for MineButton that includes a clicked signal
// that also returns the id of the base object. 
class MineButton : public QPushButton {
    Q_OBJECT
public:
    MineButton(const model::MineCoord& coord, QWidget* parent = nullptr);

private slots:
    // This slot catches the signal emitted by the base class when the button is
    // clicked. It then emits a new clicked signal with the id of the button as its
    // parameter. (QWidget/QPushButton mouse press event override)
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    // Signal for when the button is clicked (has id as parameter)
    void left_clicked(const model::MineCoord& id) const;
    void right_clicked(const model::MineCoord& id) const;

private:
    model::MineCoord m_coord;

};

} // namespace view