#pragma once

#include "view/button.h"
#include "view/ui_game.h"
#include "model/data.h"
#include <QWidget>
#include <QString>

namespace view {

    class MineWindow : public QMainWindow, private Ui::GameWindow {
        Q_OBJECT
    public:
        MineWindow() = default;
        // Constructs a game window with an initial state. The state will be pushed into a
        // QGridLayout, with the mine board buttons generated dynamically
        MineWindow(const model::MineBoard& init_state, QWidget* parent = nullptr);
        void update_board(const model::MineBoard& new_state);
        void clear_board();

    private slots:
        void on_restart() const;
        void on_reveal(const model::MineCoord& coord) const;
        void on_mark(const model::MineCoord& coord) const;

    signals:
        void restart() const;
        void reveal(const model::MineCoord& coord) const;
        void mark(const model::MineCoord& coord) const;

    private:
        // We do not need to manually deallocate the buttons because the QMainWindow class
        // does it for us. Each parent is assigned this as its parent, and when parents
        // are destroyed, Qt automatically calls the destructor on each of its children.
        std::vector<std::vector<MineButton*>> buttons;
    
    };

} // namespace view