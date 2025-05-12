#pragma once


#include <QPushButton>
#include <QMouseEvent>
#include <QWidget>

#include "model/board.h"

class ButtonView : public QPushButton {
    Q_OBJECT
public:
    explicit ButtonView(const GameBoardCoord& coord, QWidget* parent = nullptr);
    void setClickable(bool clickable);

private slots:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

signals:
    // signal when the button is clicked as a number
    void lmbPressed(const GameBoardCoord& id) const;
    // signal when the button is clicked as a regular
    void lmbReleasedInside(const GameBoardCoord& id) const;
    void lmbReleasedOutside(const GameBoardCoord& id) const;
    void rmbReleased(const GameBoardCoord& id) const;

private:
    bool m_clickable;
    GameBoardCoord m_coord;
};