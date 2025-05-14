#include <QWidget>
#include <QPixmap>

#include "view/about.h"
#include "utils/config.h"

AboutView::AboutView(QWidget* parent) : QDialog(parent) {
    m_ui = new Ui::AboutWindow();
    m_ui->setupUi(this);
    
    QPixmap icn = QPixmap(ABOUT_WINDOW_ICON_PATH);
    m_ui->app_logo->setPixmap(icn.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_ui->copyright->setText(m_ui->copyright->text() + "\n\nVersion " + TARGET_VERSION_STRING);
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

AboutView::~AboutView() {
    delete m_ui;
}