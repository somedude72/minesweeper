#pragma once

#include <QApplication>

#include "view/about.h"
#include "view/window.h"
#include "model/screen.h"
#include "model/data.h"

class App : public QApplication {
public:
    App(int argc, char** argv);
    ~App();

private:
    void revealMines(const model::MineCoord& cause);

private slots:
    void onRestart();
    void onMark(const model::MineCoord& coord);
    void onReveal(const model::MineCoord& coord);

    void onClose();
    void onMinimize();

    void onActionBeginner();
    void onActionIntermediate();
    void onActionAdvanced();
    void onActionAbout();
    void onActionCustom() const;
    void onActionSetSafeFirstClick(bool safe) const;

    void deleteAboutWindow();

private:
    model::MineBoard m_board;
    model::GameSettings m_settings;
    view::GameWindow* m_game_window;
    view::AboutWindow* m_about_window = nullptr;

    const int32_t min_size = model::Screen::getMinSize();
    bool m_game_over, m_game_won;

};