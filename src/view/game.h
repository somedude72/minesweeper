#pragma once

#include <QMainWindow>
#include <QPointF>
#include <QString>
#include <QMouseEvent>

#include "view/ui_game.h"
#include "view/button.h"
#include "model/board.h"
#include "model/data.h"
#include "model/screen.h"

class GameView : public QMainWindow, private Ui::GameWindow {
    Q_OBJECT
public:
    GameView() = default;
    explicit GameView(const GameBoard& init_board, QWidget* parent = nullptr);

    // the updatewindow handles the case when new_board has the same size as the current
    // window. resizewindow handles the case when new_board has a different size. the
    // reason we want to make this distinction is because deleting the old button objects
    // and reallocating new ones are expensive. hence, we want to provide two functions to
    // handle both cases. 
    void updateBoard(const GameBoard& board, const GameState& state, bool first_render = false);
    void initBoard(const GameBoard& board, const GameState& state, bool first_render = false);

    void setMinesLeft(int new_mines);
    void setTimePassed(int new_time);

protected:
    // qt custom title bar movement implementation
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void onEnableSurpriseFace(const GameBoardCoord& coord);
    void onDisableSurpriseFace(const GameBoardCoord& coord);

    // game slots
    void onRestart() const;
    void onReveal(const GameBoardCoord& coord) const;
    void onMark(const GameBoardCoord& coord) const;

    // menu slots
    void onActionBeginner() const;
    void onActionIntermediate() const;
    void onActionAdvanced() const;
    void onActionAbout() const;
    void onActionOptions() const;
    void onActionTutorial() const;
    void onActionGithub() const;

    // title bar slots
    void onClose() const;
    void onMinimize() const;

signals:
    void restart() const;
    void reveal(const GameBoardCoord& coord) const;
    void mark(const GameBoardCoord& coord) const;

    void actionBeginner() const;
    void actionIntermediate() const;
    void actionAdvanced() const;
    void actionAbout() const;
    void actionOptions() const;
    void actionTutorial() const;
    void actionGithub() const;

    void close() const;
    void minimize() const;

private:
    void setupMenu();
    void setupFontAndIcons();
    void clearBoard();
    void updateControlIcon(const GameState& state) const;
    void renderButton(const GameBoardSquare& square, const GameState& new_state, ButtonView* button_view) const;

private:
    GameState m_prev_state = { false, false, false };
    GameBoard m_prev_board = GameBoard();
    
    std::vector<std::vector<ButtonView*>> m_buttons = {{ nullptr }};
    QString m_board_font, m_window_font;
    QIcon m_flag, m_mine, m_wrong_mine, m_no_icon;

private:
    // custom title bar implementation partially taken from
    // https://stackoverflow.com/questions/11314429/select-moving-qwidget-in-the-screen
    QPointF m_prev_position;
    bool m_moving_window = false;
    const int32_t m_min_size = minScreenSize();
};