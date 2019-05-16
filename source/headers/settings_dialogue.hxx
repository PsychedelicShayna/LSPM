#ifndef SETTINGS_DIALOGUE_HXX
#define SETTINGS_DIALOGUE_HXX

#include <QMessageBox>
#include <QFileDialog>
#include <QDialog>

#include <experimental/filesystem>
#include <functional>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include "json.txx"

namespace fs = std::experimental::filesystem;
using Json = nlohmann::json;

namespace Ui {
    class SettingsDialogue;
}

class SettingsDialogue : public QDialog {
private:
    Ui::SettingsDialogue* ui;
    Q_OBJECT

    std::function<void(Json)> callback_;
    QWidget* parent_;

    void closeEvent(QCloseEvent*) override;
    void reject() override;

private slots:
    void on_comboBox_EncryptionMode_currentIndexChanged(int);

    void on_toolButton_BrowseStyle_clicked();
    void on_toolButton_BrowseStartupWallet_clicked();

    void on_pushButton_SaveSettings_clicked();

    void on_lineEdit_StylePath_textChanged(const QString&);
    void on_lineEdit_StartupWalletPath_textChanged(const QString&);

public:
    explicit SettingsDialogue(std::function<void(Json)>, QWidget* parent);
    ~SettingsDialogue() override;

};

#endif // SETTINGS_DIALOGUE_HXX
