#include "save_file_dialogue.hxx"
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

    if(this->selectedAesMode_ == NULL) {
        try{ this->targetWallet_.DumpFile(file_path); }
        catch(const std::invalid_argument& exception) {
            static QMessageBox* messagebox_error = nullptr;
            if(messagebox_error != nullptr) delete messagebox_error;
            messagebox_error = new QMessageBox(this);
            messagebox_error->setText(exception.what());
            messagebox_error->show();
            return;
        }

        this->close();
    } else {
        BasicAes basicAes;
        std::string raw_encryption_key = this->ui->lineEdit_EncryptionKey->text().toStdString();

        if(!raw_encryption_key.size()) {
            static QMessageBox* messagebox_error = nullptr;
            if(messagebox_error != nullptr) delete messagebox_error;
            messagebox_error = new QMessageBox(this);
            messagebox_error->setText("Please specify an encryption key.");
            messagebox_error->show();
            return;
        }

        if(this->selectedKeyMode_ == KEY_MODE::DIGEST) {
            switch(this->selectedAesMode_) {
            case BasicAes::AES_128 :
                basicAes.LoadKey<BasicAes::AES_128>(raw_encryption_key);
                break;

            case BasicAes::AES_192 :
                basicAes.LoadKey<BasicAes::AES_192>(raw_encryption_key);
                break;

            case BasicAes::AES_256 :
                basicAes.LoadKey<BasicAes::AES_256>(raw_encryption_key);
                break;
            }
        }
        else if(this->selectedKeyMode_ == KEY_MODE::DIRECT) {
            if(raw_encryption_key.size() == this->selectedAesMode_) {
                switch(this->selectedAesMode_) {
                case BasicAes::AES_128 : {
                    std::array<uint8_t, BasicAes::AES_128> raw_encryption_key_array;
                    memcpy(raw_encryption_key_array.data(), raw_encryption_key.data(), BasicAes::AES_128);
                    basicAes.LoadKeyDirect<BasicAes::AES_128>(raw_encryption_key_array);
                    break;
                }

                case BasicAes::AES_192 : {
                    std::array<uint8_t, BasicAes::AES_192> raw_encryption_key_array;
                    memcpy(raw_encryption_key_array.data(), raw_encryption_key.data(), BasicAes::AES_192);
                    basicAes.LoadKeyDirect<BasicAes::AES_192>(raw_encryption_key_array);
                    break;
                }

                case BasicAes::AES_256 : {
                    std::array<uint8_t, BasicAes::AES_256> raw_encryption_key_array;
                    memcpy(raw_encryption_key_array.data(), raw_encryption_key.data(), BasicAes::AES_256);
                    basicAes.LoadKeyDirect<BasicAes::AES_256>(raw_encryption_key_array);
                    break;
                }
                }
            } else {
                static QMessageBox* messagebox_error = nullptr;
                if(messagebox_error != nullptr) delete messagebox_error;
                messagebox_error = new QMessageBox(this);
                messagebox_error->setText(QString("The size of the encryption key must be ").append(QString::number(this->selectedAesMode_)).append(" bytes."));
                messagebox_error->show();
                return;
            }
        }

        std::vector<uint8_t> encrypted_wallet_data = basicAes.Encrypt(this->targetWallet_.DumpDirect());

        std::ofstream output_stream(file_path, std::ios::binary | std::ios::trunc);
        if(output_stream.good()) {
            output_stream.write(
                reinterpret_cast<char*>(encrypted_wallet_data.data()),
                static_cast<std::streamsize>(encrypted_wallet_data.size())
            );

            output_stream.close();
            this->close();
        } else {
            static QMessageBox* messagebox_error = nullptr;
            if(messagebox_error != nullptr) delete messagebox_error;
            messagebox_error = new QMessageBox(this);
            messagebox_error->setText("Could not write to file. The stream wasn't good.");
            messagebox_error->show();
            return;
        }
    }
}

void SaveFileDialogue::on_comboBox_EncryptionMode_currentIndexChanged(int index) {
    switch(index) {
    case 1 :
        this->selectedAesMode_ = BasicAes::AES_128;
        this->ui->comboBox_KeyMode->setEnabled(true);
        this->ui->lineEdit_EncryptionKey->setEnabled(true);
        break;

    case 2 :
        this->selectedAesMode_ = BasicAes::AES_192;
        this->ui->comboBox_KeyMode->setEnabled(true);
        this->ui->lineEdit_EncryptionKey->setEnabled(true);
        break;

    case 3 :
        this->selectedAesMode_ = BasicAes::AES_256;
        this->ui->comboBox_KeyMode->setEnabled(true);
        this->ui->lineEdit_EncryptionKey->setEnabled(true);
        break;

    default:
        this->selectedAesMode_ = static_cast<BasicAes::AES_MODE>(NULL);
        this->ui->comboBox_KeyMode->setEnabled(false);
        this->ui->lineEdit_EncryptionKey->setEnabled(false);
        break;
    };
}

void SaveFileDialogue::on_comboBox_KeyMode_currentIndexChanged(int index) {
    this->selectedKeyMode_ = static_cast<KEY_MODE>(index);
}

void SaveFileDialogue::closeEvent(QCloseEvent*) {
    this->walletSavedCallback_();
}

void SaveFileDialogue::reject() {
    this->walletSavedCallback_();
}

SaveFileDialogue::SaveFileDialogue(Wallet targetWallet, std::function<void()> walletSavedCallback, QWidget* parent) : QDialog(parent), ui(new Ui::SaveFileDialogue), TargetWallet(targetWallet_) {
    ui->setupUi(this);

    this->walletSavedCallback_ = walletSavedCallback;
    this->targetWallet_ = targetWallet;

    this->selectedAesMode_ = static_cast<BasicAes::AES_MODE>(NULL);
    this->selectedKeyMode_ = KEY_MODE::DIGEST;
}

SaveFileDialogue::~SaveFileDialogue() {
    delete this->ui;
}