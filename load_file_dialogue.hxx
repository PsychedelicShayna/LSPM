#ifndef LOAD_FILE_DIALOGUE_HXX
#define LOAD_FILE_DIALOGUE_HXX

#include <QFileDialog>
#include <QMessageBox>
#include <QDialog>

#include <functional>

#include "basic_aes.hxx"
#include "wallet.hxx"

namespace Ui {
    class LoadFileDialogue;
}

enum struct KEY_MODE {
    DIGEST = 0, DIRECT = 1
};

struct EncryptionData {
    KEY_MODE KeyMode;
    BasicAes::AES_MODE AesMode;
    std::string EncryptionKey;
    std::string WalletPath;

    EncryptionData() = default;
};

class LoadFileDialogue : public QDialog {
private:
    Ui::LoadFileDialogue* ui;
    Q_OBJECT

    std::function<void(Wallet, bool, EncryptionData*)> walletLoadedCallback_;
    Wallet targetWallet_;

    BasicAes::AES_MODE selectedAesMode_;
    KEY_MODE selectedKeyMode_;

private slots:
    void on_toolButton_Browse_clicked();

    void on_comboBox_AESMode_currentIndexChanged(int);
    void on_comboBox_KeyMode_currentIndexChanged(int);

    void on_pushButton_LoadFile_clicked();

    void closeEvent(QCloseEvent*) override;
    void reject() override;

public:
    void SetDefaultState(const std::string&, int, int);

    explicit LoadFileDialogue(std::function<void(Wallet, bool, EncryptionData*)>, QWidget*);
    ~LoadFileDialogue() override;
};

#endif // LOAD_FILE_DIALOGUE_HXX
