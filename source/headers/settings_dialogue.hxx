#ifndef SETTINGS_DIALOGUE_HXX
#define SETTINGS_DIALOGUE_HXX

#include <QMessageBox>
#include <QFileDialog>
#include <QDialog>

#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "json.txx"

using Json = nlohmann::json;

namespace Ui {
    class SettingsDialogue;
}

class SettingsDialogue : public QDialog {
private:
    Ui::SettingsDialogue* ui;
    Q_OBJECT

    std::function<void(Json)> callback_;

private slots:
    void on_comboBox_EncryptionMode_currentIndexChanged(int);

    void on_toolButton_BrowseStyle_clicked();
    void on_toolButton_BrowseStartupWallet_clicked();

    void on_pushButton_SaveSettings_clicked();

public:
    explicit SettingsDialogue(std::function<void(Json)>, QWidget* parent);
    ~SettingsDialogue() override;

};

#endif // SETTINGS_DIALOGUE_HXX
