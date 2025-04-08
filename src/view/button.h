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
    void setClickableUi(bool clickable);

private slots:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    void enableSurpriseFace() const;
    void disableSurpriseFace() const;
    // Signal for when the button is clicked (has id as parameter)
    void lmbReleased(const model::MineCoord& id) const;
    void rmbReleased(const model::MineCoord& id) const;

private:
    bool m_clickable;
    model::MineCoord m_coord;

};

} // namespace view