#include "load_file_dialogue.hxx"
#include "ui_load_file_dialogue.h"

void LoadFileDialogue::on_toolButton_Browse_clicked() {
    static QFileDialog* file_dialogue = nullptr;
    if(file_dialogue != nullptr)  delete file_dialogue;

    file_dialogue = new QFileDialog();

    const QString& file_path = file_dialogue->getOpenFileName(this, "Wallet Location", "", "*.uepm");
    this->ui->lineEdit_FilePath->setText(file_path);
}

void LoadFileDialogue::on_comboBox_AESMode_currentIndexChanged(int index) {
    if(index > 0) {
        this->ui->comboBox_KeyMode->setEnabled(true);
        this->ui->lineEdit_EncryptionKey->setEnabled(true);

        switch(index) {
            case 1 : {
                this->selectedAesMode_ = BasicAes::AES_128;
                break;
            }

            case 2 : {
                this->selectedAesMode_ = BasicAes::AES_192;
                break;
            }

            case 3 : {
                this->selectedAesMode_ = BasicAes::AES_256;
                break;
            }
        }
    } else {
        this->selectedAesMode_ = static_cast<BasicAes::AES_MODE>(NULL);
        this->ui->comboBox_KeyMode->setEnabled(false);
        this->ui->lineEdit_EncryptionKey->setEnabled(false);
    }
}

void LoadFileDialogue::on_comboBox_KeyMode_currentIndexChanged(int index) {
    switch(index) {
        case 0 : {
            this->selectedKeyMode_ = KEY_MODE::DIGEST;
            break;
        }

        case 1 : {
            this->selectedKeyMode_ = KEY_MODE::DIRECT;
            break;
        }
    }
}

void LoadFileDialogue::on_pushButton_LoadFile_clicked() {
    const std::string& file_path = this->ui->lineEdit_FilePath->text().toStdString();
    Wallet loaded_wallet;

    EncryptionData* encryptionData = new EncryptionData {
        this->selectedKeyMode_,
        this->selectedAesMode_,
        this->ui->lineEdit_EncryptionKey->text().toStdString(),
        file_path
    };

    if(this->selectedAesMode_ == NULL) {
        try {
            loaded_wallet.LoadFile(file_path);
            this->walletLoadedCallback_(loaded_wallet, true, encryptionData);
        }

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
        std::ifstream input_stream(file_path, std::ios::binary);
        if(!input_stream.good()) {
            static QMessageBox* messagebox_error = nullptr;
            if(messagebox_error != nullptr) delete messagebox_error;
            messagebox_error = new QMessageBox(this);
            messagebox_error->setText("The file probably doesn't exist (the stream wasn't good).");
            messagebox_error->show();
            return;
        }

        std::vector<uint8_t> file_data((std::istreambuf_iterator<char>(input_stream)), (std::istreambuf_iterator<char>()));
        input_stream.close();

        // Encryption Setup ---------------------------------------------------------
        BasicAes basicAes;

        // Gets the encryption key specified in the encryption key textbox.
        std::string raw_encryption_key = this->ui->lineEdit_EncryptionKey->text().toStdString();

        // Checks if the encryption key size is above zero.
        if(!raw_encryption_key.size()) {
            static QMessageBox* messagebox_error = nullptr;
            if(messagebox_error != nullptr) delete messagebox_error;
            messagebox_error = new QMessageBox(this);
            messagebox_error->setText("Please specify an encryption key.");
            messagebox_error->show();
            return;
        }

        // Checks the encryption settings, and configures the basicAes instance according to those settings.
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
        // --------------------------------------------------------------------------

        // Decrypt the wallet data. Failed decryption procudes an incorrect output.
        std::vector<uint8_t> decrypted_wallet_data = basicAes.Decrypt(file_data);

        // Attepts to load the decrypted wallet data. This should throw an exception if the decrypted output is incorrect.
        try {
            loaded_wallet.LoadDirect(decrypted_wallet_data);
            this->walletLoadedCallback_(loaded_wallet, true, encryptionData);
            this->close();
            return;
        }

        catch(const std::invalid_argument&) {
            static QMessageBox* messagebox_error = nullptr;
            if(messagebox_error != nullptr) delete messagebox_error;
            messagebox_error = new QMessageBox(this);
            messagebox_error->setText("Failed to decrypt the wallet, perhaps the key, or decryption settings are incorrect.");
            messagebox_error->show();
            return;
        }
    }
}

void LoadFileDialogue::closeEvent(QCloseEvent*) {
    this->walletLoadedCallback_(Wallet(), false, nullptr);
}

void LoadFileDialogue::reject() {
    this->walletLoadedCallback_(Wallet(), false, nullptr);
}

void LoadFileDialogue::SetDefaultState(const std::string& path, int aes_mode_index, int key_mode_index) {
    this->ui->lineEdit_FilePath->setText(QString::fromStdString(path));
    this->ui->comboBox_AESMode->setCurrentIndex(aes_mode_index);
    this->ui->comboBox_KeyMode->setCurrentIndex(key_mode_index);
}

LoadFileDialogue::LoadFileDialogue(std::function<void(Wallet, bool, EncryptionData*)> callback, QWidget* parent) : QDialog(parent), ui(new Ui::LoadFileDialogue) {
    this->selectedAesMode_ = static_cast<BasicAes::AES_MODE>(NULL);
    this->selectedKeyMode_ = static_cast<KEY_MODE>(NULL);
    this->walletLoadedCallback_ = callback;
    ui->setupUi(this);
}

LoadFileDialogue::~LoadFileDialogue() {
    delete ui;
}
