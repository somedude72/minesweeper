#pragma once

#include "view/button.h"
#include "view/ui_window.h"
#include "model/data.h"

#include "QWidget"
#include "QPoint"
#include "QString"
#include "QMouseEvent"

namespace view {

class MineWindow : public QMainWindow, private Ui::GameWindow {
    Q_OBJECT
public:
    MineWindow() = default;
    // Constructs a game window with an initial state. The state will be pushed into a
    // QGridLayout, with the mine board buttons generated dynamically
    MineWindow(const model::MineBoard& init_board, QWidget* parent = nullptr);
    void update_window(const model::MineBoard& new_board, const model::GameState& new_state);

protected:
    // Qt custom title bar movement mouse events
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    // When a square is pressed, change the face of the restart button to surprised icon
    void on_enable_surprise_face();
    void on_disable_surprise_face();

    // These slots all emit signals to app controller
    void on_restart() const;
    void on_reveal(const model::MineCoord& coord) const;
    void on_mark(const model::MineCoord& coord) const;

    void on_close() const;
    void on_minimize() const;

signals:
    void restart() const;
    void reveal(const model::MineCoord& coord) const;
    void mark(const model::MineCoord& coord) const;

    void close() const;
    void minimize() const;

private:
    void clear_board();
    void update_icon();
    void render_button(const model::MineSquare& square, MineButton* button_view);

private:
    model::GameState m_state;

    // We do not need to manually deallocate the buttons because the QMainWindow class
    // does it for us. Each parent is assigned this as its parent, and when parents
    // are destroyed, Qt automatically calls the destructor on each of its children.
    std::vector<std::vector<MineButton*>> m_buttons;

    // Custom title bar implementation partially taken from
    // https://stackoverflow.com/questions/11314429/select-moving-qwidget-in-the-screen
    QPointF m_prev_position;
    bool m_moving_window = false;

};

} // namespace view