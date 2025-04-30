#pragma once

#include <QDialog>
#include "view/ui_about.h"

namespace view {

class AboutWindow : public QDialog {
    Q_OBJECT
public:
    AboutWindow(QWidget* parent = nullptr);
    ~AboutWindow();
private:
    Ui::AboutWindow* m_ui;
};

} // namespace view