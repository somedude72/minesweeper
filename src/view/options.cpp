#include <QKeyEvent>

#include "view/options.h"

OptionsView::OptionsView(const GameSettings& settings, QWidget* parent) : QDialog(parent) {
    m_ui = new Ui::Options;
    m_ui->setupUi(this);
    m_settings = settings;
    connect(m_ui->row_slider, &QSlider::valueChanged, this, &OptionsView::onRowSliderChanged);
    connect(m_ui->col_slider, &QSlider::valueChanged, this, &OptionsView::onColSliderChanged);
    connect(m_ui->mine_slider, &QSlider::valueChanged, this, &OptionsView::onMineSliderChanged);
    connect(m_ui->row_editor, &QLineEdit::editingFinished, this, &OptionsView::onRowEditorChanged);
    connect(m_ui->col_editor, &QLineEdit::editingFinished, this, &OptionsView::onColEditorChanged);
    connect(m_ui->mine_editor, &QLineEdit::editingFinished, this, &OptionsView::onMineEditorChanged);
    connect(m_ui->clear_checkbox, &QCheckBox::checkStateChanged, this, &OptionsView::onClearChanged);
    connect(m_ui->safe_checkbox, &QCheckBox::checkStateChanged, this, &OptionsView::onSafeChanged);
    connect(this, &QDialog::accepted, this, &OptionsView::onDone);

    m_ui->row_slider->setValue(m_settings.row_size);
    m_ui->row_editor->setText(QString::number(m_settings.row_size));
    m_ui->col_slider->setValue(m_settings.col_size);
    m_ui->col_editor->setText(QString::number(m_settings.col_size));
    m_ui->mine_slider->setValue(m_settings.num_mines);
    m_ui->mine_editor->setText(QString::number(m_settings.num_mines));
    m_ui->clear_checkbox->setChecked(m_settings.is_clear_first_move);
    m_ui->safe_checkbox->setChecked(m_settings.is_safe_first_move);

    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

OptionsView::~OptionsView() {
    delete m_ui;
}

void OptionsView::enableMineCountWarning() {
    delete warn_label;
    warn_label = new QLabel("Warning: Too many mines! Automatically resizing the number of mines down to 40% of the total board.", nullptr);
    warn_label->setStyleSheet(".QLabel { color: rgb(220, 171, 23);}");
    warn_label->setWordWrap(true);
    warn_label->setAlignment(Qt::AlignHCenter);
    m_ui->verticalLayout->insertWidget(1, warn_label);
}

void OptionsView::disableMineCountWarning() {
    delete warn_label;
    warn_label = nullptr;
}

bool OptionsView::isValidMineCount() {
    return maxMineCount() >= m_settings.num_mines;
}

int32_t OptionsView::maxMineCount() {
    return 0.4 * m_settings.row_size * m_settings.col_size;
}

void OptionsView::checkValidMineCount() {
    if (!isValidMineCount()) {
        m_settings.num_mines = maxMineCount();
        m_ui->mine_editor->setText(QString::number(maxMineCount()));
        m_ui->mine_slider->setValue(maxMineCount());
        enableMineCountWarning();
    } else {
        disableMineCountWarning();
    }
}

void OptionsView::keyPressEvent(QKeyEvent* evt) {
    if(evt->key() != Qt::Key_Enter && evt->key() != Qt::Key_Return) {
        QDialog::keyPressEvent(evt);
    }
}

void OptionsView::onRowSliderChanged(int value) {
    m_ui->row_editor->setText(QString::number(value));
    m_settings.row_size = value;
    checkValidMineCount();
}

void OptionsView::onColSliderChanged(int value) {
    m_ui->col_editor->setText(QString::number(value));
    m_settings.col_size = value;
    checkValidMineCount();
}

void OptionsView::onMineSliderChanged(int value) {
    m_ui->mine_editor->setText(QString::number(value));
    m_settings.num_mines = value;
    checkValidMineCount();
}

void OptionsView::onRowEditorChanged() {
    QString value = m_ui->row_editor->text();
    if (value == "" || value.toInt() <= 9) {
        m_ui->row_slider->setValue(9);
        m_ui->row_editor->setText("9");
        m_settings.row_size = 9;
    } else if (value.toInt() >= 60) {
        m_ui->row_slider->setValue(60);
        m_ui->row_editor->setText("60");
        m_settings.row_size = 60;
    } else {
        m_ui->row_slider->setValue(value.toInt());
        m_settings.row_size = value.toInt();
    }
}

void OptionsView::onColEditorChanged() {
    QString value = m_ui->col_editor->text();
    if (value == "" || value.toInt() <= 9) {
        m_ui->col_slider->setValue(9);
        m_ui->col_editor->setText("9");
        m_settings.col_size = 9;
    } else if (value.toInt() >= 60) {
        m_ui->col_slider->setValue(60);
        m_ui->col_editor->setText("60");
        m_settings.col_size = 60;
    } else {
        m_ui->col_slider->setValue(value.toInt());
        m_settings.col_size = value.toInt();
    }
}


void OptionsView::onMineEditorChanged() {
    QString value = m_ui->mine_editor->text();
    if (value == "" || value.toInt() <= 9) {
        m_ui->mine_slider->setValue(9);
        m_ui->mine_editor->setText("9");
        m_settings.num_mines = 9;
    } else if (value.toInt() >= 499) {
        m_ui->mine_slider->setValue(499);
        m_ui->mine_editor->setText("499");
        m_settings.num_mines = 499;
    } else {
        m_ui->mine_slider->setValue(value.toInt());
        m_settings.num_mines = value.toInt();
    }
}


void OptionsView::onSafeChanged(Qt::CheckState value) {
    if (value == Qt::CheckState::Checked) {
        m_settings.is_safe_first_move = true;
    } else {
        m_settings.is_safe_first_move = false;
        m_settings.is_clear_first_move = false;
        m_ui->clear_checkbox->setChecked(false);
    }
}

void OptionsView::onClearChanged(Qt::CheckState value) {
    if (value == Qt::CheckState::Checked) {
        m_settings.is_clear_first_move = true;
        m_settings.is_safe_first_move = true;
        m_ui->safe_checkbox->setChecked(true);
    } else {
        m_settings.is_clear_first_move = false;
    }
}

void OptionsView::onDone() {
    if (!isValidMineCount())
        m_settings.num_mines = (int32_t) 0.4 * m_settings.row_size * m_settings.col_size;
    emit applySettings(m_settings);
}