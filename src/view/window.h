#pragma once

#include "view/button.h"
#include "view/ui_window.h"
#include "model/data.h"
#include "model/screen.h"

#include <QWidget>
#include <QPoint>
#include <QString>
#include <QMainWindow>
#include <QMouseEvent>

namespace view {

class GameWindow : public QMainWindow, private Ui::GameWindow {
    Q_OBJECT
public:
    GameWindow() = default;
    // Constructs a game window with an initial state. The state will be pushed into a
    // QGridLayout, with the mine board buttons generated dynamically
    GameWindow(const model::MineBoard& init_board, QWidget* parent = nullptr);
    // The updateWindow handles the case when new_board has the same size as the current
    // window. resizeWindow handles the case when new_board has a different size. The
    // reason we want to make this distinction is because deleting the old button objects
    // and reallocating new ones are expensive. Hence, we want to provide two functions to
    // handle both cases. 
    void updateBoard(const model::MineBoard& new_board, const model::GameState& new_state, bool first_render = false);
    void initBoard(const model::MineBoard& new_board, const model::GameState& new_state, bool first_render = false);

protected:
    // Qt custom title bar movement mouse events
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    // When a square is pressed, change the face of the restart button to surprised icon
    void onEnableSurpriseFace();
    void onDisableSurpriseFace();

    // These slots all emit signals to app controller
    void onRestart() const;
    void onReveal(const model::MineCoord& coord) const;
    void onMark(const model::MineCoord& coord) const;

    void onActionBeginner() const;
    void onActionIntermediate() const;
    void onActionAdvanced() const;
    void onActionAbout() const;
    void onActionCustom() const;
    void onActionSetSafeFirstClick(bool safe) const;

    void onClose() const;
    void onMinimize() const;

signals:
    void restart() const;
    void reveal(const model::MineCoord& coord) const;
    void mark(const model::MineCoord& coord) const;

    void actionBeginner() const;
    void actionIntermediate() const;
    void actionAdvanced() const;
    void actionAbout() const;
    void actionCustom() const;
    void actionSetSafeFirstClick(bool safe) const;

    void close() const;
    void minimize() const;

private:
    void setupFontAndIcons();
    void setupMenu();
    void updateControlIcon(const model::GameState& state);
    void renderButton(const model::MineSquare& square, const model::GameState& state, MineButton* button_view);

private:
    model::GameState m_prev_state;
    model::MineBoard m_prev_board;
    
    std::vector<std::vector<MineButton*>> m_buttons;
    QString m_board_font, m_window_font;
    QIcon m_flag, m_mine, m_wrong_mine, m_no_icon;

private:
    // Custom title bar implementation partially taken from
    // https://stackoverflow.com/questions/11314429/select-moving-qwidget-in-the-screen
    QPointF m_prev_position;
    bool m_moving_window = false;
    const int32_t m_min_size = model::Screen::getMinSize();
};

} // namespace view