#ifndef SAVE_FILE_DIALOGUE_HXX
#define SAVE_FILE_DIALOGUE_HXX

#include <experimental/filesystem>

#include <iostream>
#include <sstream>
#include <cstdint>
#include <fstream>
#include <vector>
#include <array>

#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>

#include "basic_aes.hxx"
#include "wallet.hxx"
#include "macros.hxx"

namespace fs = std::experimental::filesystem;

namespace Ui {
    class SaveFileDialogue;
}

class SaveFileDialogue : public QDialog {
private:
    Ui::SaveFileDialogue* ui;
    Q_OBJECT

    std::function<void()> walletSavedCallback_;
    Wallet targetWallet_;

private slots:
    void on_toolButton_BrowsePath_clicked();
    void on_pushButton_SaveWallet_clicked();

    void on_comboBox_EncryptionMode_currentIndexChanged(int);

    void closeEvent(QCloseEvent*) override;
    void reject() override;

public:
    const Wallet& TargetWallet;

    explicit SaveFileDialogue(Wallet, std::function<void()>, QWidget*);
    ~SaveFileDialogue() override;
};

#endif // SAVE_FILE_DIALOGUE_HXX
