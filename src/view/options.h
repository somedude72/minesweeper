#include <QDialog>
#include <QWidget> 
#include <QString>

#include "model/data.h"
#include "view/ui_options.h"

class OptionsView : public QDialog {
    Q_OBJECT
public:
    explicit OptionsView(const GameSettings& settings, QWidget* parent = nullptr);
    ~OptionsView();

private:
    void enableMineCountWarning();
    void disableMineCountWarning();
    bool isValidMineCount();
    int32_t maxMineCount();

    void checkValidMineCount();

protected:
    // we do not want the enter key to close our qdialog
    void keyPressEvent(QKeyEvent* evt) override;

private slots:
    void onRowSliderChanged(int value);
    void onColSliderChanged(int value);
    void onMineSliderChanged(int value);
    void onRowEditorChanged();
    void onColEditorChanged();
    void onMineEditorChanged();

    void onSafeChanged(Qt::CheckState value);
    void onClearChanged(Qt::CheckState value);
    void onDone();

signals:
    void applySettings(const GameSettings& settings) const;

private:
    Ui::Options* m_ui;
    QLabel* warn_label = nullptr;
    GameSettings m_settings = GameSettings();
};