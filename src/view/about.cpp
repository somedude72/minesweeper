#include "view/about.h"
#include <QWidget>
#include "view/ui_about.h"

namespace view {

AboutWindow::AboutWindow(QWidget* parent) : QDialog(parent) {
    m_ui = new Ui::AboutWindow();
    m_ui->setupUi(this);
#if defined(__APPLE__)
    m_ui->app_logo->setPixmap(QPixmap(":/assets/macos/mac-icon.png").scaled(
        160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
#else
    m_ui->app_logo->setPixmap(QPixmap(":/assets/icon.png").scaled(
        160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
#endif
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

AboutWindow::~AboutWindow() {
    delete m_ui;
}

} // namespace view