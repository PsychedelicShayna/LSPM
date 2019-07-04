#ifndef LOAD_FILE_DIALOGUE_HXX
#define LOAD_FILE_DIALOGUE_HXX

#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>

#include <experimental/filesystem>
#include <functional>

#include "basic_aes.hxx"
#include "wallet.hxx"
#include "macros.hxx"

namespace fs = std::experimental::filesystem;

namespace Ui {
    class LoadFileDialogue;
}

class LoadFileDialogue : public QDialog {
private:
    Ui::LoadFileDialogue* ui;
    Q_OBJECT

    std::function<void(Wallet, std::string, AesCredentials)> walletLoadedCallback_;
    QWidget* parent_;

    void closeEvent(QCloseEvent*) override;
    void reject() override;

private slots:
    void on_comboBox_AESMode_currentIndexChanged(int);
    void on_pushButton_LoadFile_clicked();
    void on_toolButton_Browse_clicked();

    void on_lineEdit_FilePath_textChanged(const QString&);

public:
    struct LoadFileDialogueState {
        std::string lineEdit_FilePath_Text;
        int comboBox_AESMode_Index;
        int comboBox_KeyMode_Index;
    };

    void LoadState(LoadFileDialogueState);

    explicit LoadFileDialogue(std::function<void(Wallet, std::string, AesCredentials)>, QWidget*);
    ~LoadFileDialogue() override;
};

#endif // LOAD_FILE_DIALOGUE_HXX
