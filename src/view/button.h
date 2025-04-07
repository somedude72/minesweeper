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
    void set_clickable_ui(bool clickable);

private slots:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void enable_surprise_face() const;
    void disable_surprise_face() const;
    // Signal for when the button is clicked (has id as parameter)
    void lmb_released(const model::MineCoord& id) const;
    void rmb_released(const model::MineCoord& id) const;

private:
    bool m_clickable;
    model::MineCoord m_coord;

};

} // namespace view