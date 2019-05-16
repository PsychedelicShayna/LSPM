#include "headers/save_file_dialogue.hxx"
#include "ui_save_file_dialogue.h"

void SaveFileDialogue::on_toolButton_BrowsePath_clicked() {
    static QFileDialog* file_dialogue = nullptr;

    if(file_dialogue != nullptr) {
        delete file_dialogue;
    }

    file_dialogue = new QFileDialog();
    const QString& file_path = file_dialogue->getSaveFileName(this, "Wallet Location", "", "*.uepm");
    this->ui->lineEdit_FilePath->setText(file_path);
}

void SaveFileDialogue::on_pushButton_SaveWallet_clicked() {
    const std::string& file_path = this->ui->lineEdit_FilePath->text().toStdString();

    std::vector<uint8_t> wallet_data = this->targetWallet_.DumpDirect();

    if(this->ui->comboBox_EncryptionMode->currentIndex()) {
        BasicAes basic_aes;
        AES_MODE aes_mode = static_cast<AES_MODE>((this->ui->comboBox_EncryptionMode->currentIndex()*8)+8);

        std::string encryption_key_string = this->ui->lineEdit_EncryptionKey->text().toStdString();
        std::vector<uint8_t> encryption_key(encryption_key_string.begin(), encryption_key_string.end());

        try {
            if(this->ui->comboBox_KeyMode->currentIndex()) basic_aes.LoadKey(encryption_key, aes_mode);
            else basic_aes.LoadKeySha256(encryption_key, aes_mode);
        } catch(const std::invalid_argument& exception) {
            mbalert(exception.what(), this);
            return;
        }

        wallet_data = basic_aes.Encrypt(wallet_data);
    }

    std::ofstream output_stream(file_path, std::ios::trunc | std::ios::binary);
    if(output_stream.good()) {
        output_stream.write(reinterpret_cast<const char*>(wallet_data.data()), static_cast<std::streamsize>(wallet_data.size()));
        output_stream.close();
        this->close();
    } else {
        mbalert("The stream to the file wasn't good.", this);
    }
}

void SaveFileDialogue::on_comboBox_EncryptionMode_currentIndexChanged(int index) {
    this->ui->lineEdit_EncryptionKey->setEnabled(index);
    this->ui->comboBox_KeyMode->setEnabled(index);
}

void SaveFileDialogue::closeEvent(QCloseEvent*) {
    this->parent_->setEnabled(true);
}

void SaveFileDialogue::reject() {
    this->parent_->setEnabled(true);
}

SaveFileDialogue::SaveFileDialogue(Wallet targetWallet, QWidget* parent) : QDialog(parent), ui(new Ui::SaveFileDialogue), TargetWallet(targetWallet_) {
    ui->setupUi(this);
    this->parent_ = parent;

    parent->setEnabled(false);
    this->setEnabled(true);

    this->targetWallet_ = targetWallet;
}

SaveFileDialogue::~SaveFileDialogue() {
    delete this->ui;
}