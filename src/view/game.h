#pragma once

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
        void update(const model::MineBoard& new_state);

    private slots:
        void on_restart() const;
        void on_reveal(const QString& id) const;

    signals:
        void restart() const;
        void reveal(const model::MineCoord& coord) const;
    
    };

} // namespace view