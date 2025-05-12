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

    int32_t maxMineCount();
    bool isValidMineCount();
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
    void onSeedEditorChanged();
    
    void onSafeCheckChanged(Qt::CheckState value);
    void onClearCheckChanged(Qt::CheckState value);
    void onMarkCheckChanged(Qt::CheckState value);
    void onSeedCheckChanged(Qt::CheckState value);
    void onDone();

signals:
    void applySettings(const GameSettings& settings) const;

private:
    Ui::Options* m_ui;
    QLabel* warn_label = nullptr;
    GameSettings m_settings = GameSettings();
};