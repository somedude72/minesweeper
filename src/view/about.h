#pragma once

#include <QDialog>
#include "view/ui_about.h"

class AboutView : public QDialog {
    Q_OBJECT
public:
    explicit AboutView(QWidget* parent = nullptr);
    ~AboutView();
    
private:
    Ui::AboutWindow* m_ui;
};