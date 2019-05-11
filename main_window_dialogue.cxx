#include "main_window_dialogue.hxx"
#include "ui_main_window_dialogue.h"


bool MainWindow::LoadQStyleSheet(const std::string& name) {
    QFile style_sheet_file(QString::fromStdString(name));
    if(!style_sheet_file.exists()) return false;

    style_sheet_file.open(QFile::ReadOnly | QFile::Text);
    QTextStream file_stream(&style_sheet_file);

    QString style_sheet = file_stream.readAll();
    this->setStyleSheet(style_sheet);

    return true;
}

void MainWindow::on_pushButton_CreateAccount_clicked() {
    static NewAccountDialogue* new_account_dialogue = nullptr;

    if(new_account_dialogue != nullptr) {
        delete new_account_dialogue;
    }

    const std::function<void(Wallet)>& call_back = [this](Wallet modifiedWallet) ->void {
        this->loadedWallet_ = modifiedWallet;

        this->UpdateListWidgets();
        this->SetModificationAllowed(true);
    };

    new_account_dialogue = new NewAccountDialogue(this->loadedWallet_, call_back, nullptr, this);
    this->SetModificationAllowed(false);
    new_account_dialogue->show();
}

void MainWindow::on_pushButton_DeleteAccount_clicked() {
    if(this->ui->listWidget_Accounts->selectedItems().count()) {
        this->loadedWallet_.DeleteAccount(this->ui->listWidget_Accounts->selectedItems().at(0)->text().toStdString());
        this->UpdateListWidgets();
    }
}

void MainWindow::on_listWidget_Accounts_itemSelectionChanged() {
    if(this->ui->listWidget_Accounts->selectedItems().size()) {
        std::string account = this->ui->listWidget_Accounts->selectedItems().at(0)->text().toStdString();
        this->ui->listWidget_Keys->clear();

        for(const auto& pair : this->loadedWallet_.GetAccountEntries(account)) {
            QString item = QString::fromStdString(pair.first);
            this->ui->listWidget_Keys->addItem(item);
        }
    }
}

void MainWindow::on_listWidget_Keys_itemSelectionChanged() {
    if(this->ui->listWidget_Accounts->selectedItems().size() && this->ui->listWidget_Keys->selectedItems().size()) {
        std::string account = this->ui->listWidget_Accounts->selectedItems().at(0)->text().toStdString();
        std::string key = this->ui->listWidget_Keys->selectedItems().at(0)->text().toStdString();
        this->ui->lineEdit_SelectedKeyValue->clear();

        QString value = QString::fromStdString(this->loadedWallet_.WalletData.at(account).at(key));
        this->ui->lineEdit_SelectedKeyValue->setText(value);
    }
}

void MainWindow::on_listWidget_Accounts_itemDoubleClicked(QListWidgetItem* item) {
    static NewAccountDialogue* new_account_dialogue = nullptr;

    const std::string account_name = item->text().toStdString();

    const std::function<void(Wallet)>& call_back = [this](Wallet wallet) ->void {
        this->loadedWallet_ = wallet;
        this->SetModificationAllowed(true);
        this->UpdateListWidgets();
    };

    if(new_account_dialogue != nullptr) {
        delete new_account_dialogue;
    };

    new_account_dialogue = new NewAccountDialogue(this->loadedWallet_, call_back, &account_name, this);
    this->SetModificationAllowed(false);
    new_account_dialogue->show();
}

void MainWindow::on_action_PasswordGenerator_triggered() {
    static PasswordGeneratorDialogue* password_generator_dialogue = nullptr;

    if(password_generator_dialogue != nullptr) {
        delete password_generator_dialogue;
    }

    password_generator_dialogue = new PasswordGeneratorDialogue(this);
    password_generator_dialogue->show();
}

void MainWindow::on_action_Load_triggered() {
    static LoadFileDialogue* load_file_dialogue = nullptr;
    if(load_file_dialogue != nullptr) delete load_file_dialogue;

    std::function<void(Wallet, bool, EncryptionData*)> callback = [this](Wallet loadedWallet, bool success, EncryptionData* encryptionData) {
        this->SetModificationAllowed(true);

        if(success) {
            if(this->encryptionData_ != nullptr) delete this->encryptionData_;
            this->encryptionData_ = encryptionData;

            this->loadedWallet_ = loadedWallet;
            this->UpdateListWidgets();
        }
    };

    load_file_dialogue = new LoadFileDialogue(callback, this);
    this->SetModificationAllowed(false);
    load_file_dialogue->show();
}

void MainWindow::on_action_Save_triggered() {
    if(this->encryptionData_ == nullptr) {
        static QMessageBox* error_message_box = nullptr;
        if(error_message_box != nullptr) delete error_message_box;
        error_message_box = new QMessageBox(this);
        error_message_box->setText("Please load a wallet first.");
        error_message_box->show();
        return;
    }

    if(this->encryptionData_->AesMode == NULL) {

        try{ this->loadedWallet_.DumpFile(this->encryptionData_->WalletPath); }
        catch(const std::invalid_argument& exception) {
            static QMessageBox* messagebox_error = nullptr;
            if(messagebox_error != nullptr) delete messagebox_error;
            messagebox_error = new QMessageBox(this);
            messagebox_error->setText(exception.what());
            messagebox_error->show();
            return;
        }
    } else {
        BasicAes basicAes;
        std::string raw_encryption_key = this->encryptionData_->EncryptionKey;

        if(this->encryptionData_->KeyMode == KEY_MODE::DIGEST) {
            switch(this->encryptionData_->AesMode) {
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
        else if(this->encryptionData_->KeyMode == KEY_MODE::DIRECT) {
            if(raw_encryption_key.size() == this->encryptionData_->AesMode) {
                switch(this->encryptionData_->AesMode) {
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
                messagebox_error->setText(QString("The size of the encryption key must be ").append(QString::number(this->encryptionData_->AesMode)).append(" bytes."));
                messagebox_error->show();
                return;
            }
        }

        std::vector<uint8_t> encrypted_wallet_data = basicAes.Encrypt(this->loadedWallet_.DumpDirect());

        std::ofstream output_stream(this->encryptionData_->WalletPath, std::ios::binary | std::ios::trunc);
        if(output_stream.good()) {
            output_stream.write(
                reinterpret_cast<char*>(encrypted_wallet_data.data()),
                static_cast<std::streamsize>(encrypted_wallet_data.size())
            );

            output_stream.close();

            static QMessageBox* messagebox_notice = nullptr;
            if(messagebox_notice != nullptr) delete messagebox_notice;
            messagebox_notice = new QMessageBox(this);
            messagebox_notice->setText("Wallet has been saved.");
            messagebox_notice->show();
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

void MainWindow::on_action_SaveAs_triggered() {
    static SaveFileDialogue* save_file_dialogue = nullptr;

    if(save_file_dialogue != nullptr) {
        delete save_file_dialogue;
    }

    std::function<void()> callback = [this](){this->SetModificationAllowed(true);};

    save_file_dialogue = new SaveFileDialogue(this->loadedWallet_, callback, this);
    this->SetModificationAllowed(false);
    save_file_dialogue->show();
}

void MainWindow::on_action_Settings_triggered() {
    static SettingsDialogue* settings_dialogue = nullptr;
    if(settings_dialogue != nullptr) delete settings_dialogue;

    std::function<void(Json*)> callback = [this](Json* json) -> void {
        this->SetModificationAllowed(true);

        if(json != nullptr) {
            if(!(this->LoadQStyleSheet((*json)["style"]))) {
                std::cerr << "Error when loading stylesheet." << std::endl;
            }

            delete json;
        }
    };

    settings_dialogue = new SettingsDialogue(callback, this);
    this->SetModificationAllowed(false);
    settings_dialogue->show();
}

void MainWindow::SetModificationAllowed(bool state) {
    this->ui->pushButton_CreateAccount->setEnabled(state);
    this->ui->pushButton_DeleteAccount->setEnabled(state);
    this->ui->menuFile->setEnabled(state);
}

void MainWindow::UpdateListWidgets() {
    this->ui->listWidget_Accounts->clear();
    this->ui->listWidget_Keys->clear();
    this->ui->lineEdit_SelectedKeyValue->clear();

    for(auto const& account_name : this->loadedWallet_.GetAccountNames()) {
        this->ui->listWidget_Accounts->addItem(QString::fromStdString(account_name));
    }
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setWindowIcon(QIcon("./Icon.png"));

    this->encryptionData_ = nullptr;

    std::ifstream input_stream("uepm_config.json", std::ios::binary);

    if(input_stream.good()) {
        try {
            std::string file_data((std::istreambuf_iterator<char>(input_stream)), (std::istreambuf_iterator<char>()));
            Json json_data = Json::parse(file_data);

            std::string startup_wallet_path = "";
            int startup_wallet_keymode = 0;
            int startup_wallet_aesmode = 0;
            uint32_t startup_wallet_qualifier = 0;

            for(const auto& pair : json_data.items()) {
                const std::string& setting = pair.key();
                const std::string& value = pair.value();

                if(setting == "swetype") {
                    startup_wallet_aesmode = std::stoi(value);
                    ++startup_wallet_qualifier;
                }

                if(setting == "swktype") {
                    startup_wallet_keymode = std::stoi(value);
                    ++startup_wallet_qualifier;
                }

                if(setting == "swpath") {
                    startup_wallet_path = value;
                    ++startup_wallet_qualifier;
                }

                if(setting == "style") {
                    if(!(this->LoadQStyleSheet(value))) {
                        std::cerr << "Error when loading stylesheet: " << value << std::endl;
                    }
                }
            }

            if(startup_wallet_qualifier == 3) {
                std::function<void(Wallet, bool, EncryptionData*)> callback = [this](Wallet loadedWallet, bool success, EncryptionData* encryptionData) {
                    this->SetModificationAllowed(true);

                    if(success) {
                        if(encryptionData != nullptr) {
                            if(this->encryptionData_ != nullptr) delete this->encryptionData_;
                            this->encryptionData_ = encryptionData;
                        }

                        this->loadedWallet_ = loadedWallet;
                        this->UpdateListWidgets();
                    }
                };

                static LoadFileDialogue* load_file_dialogue = nullptr;
                if(load_file_dialogue != nullptr) delete load_file_dialogue;
                load_file_dialogue = new LoadFileDialogue(callback, this);
                load_file_dialogue->SetDefaultState(startup_wallet_path, startup_wallet_aesmode, startup_wallet_keymode);
                this->SetModificationAllowed(false);
                load_file_dialogue->show();
            }

        } catch (const std::exception& exception) {
            std::cerr << "Corrupt config file, exception -- " << exception.what() << std::endl;
        }
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_lineEdit_AccountFilter_textChanged(const QString& text) {
    this->ui->listWidget_Accounts->clear();
    this->ui->listWidget_Keys->clear();
    this->ui->lineEdit_SelectedKeyValue->clear();

    std::string filter = text.toStdString();
    std::transform(filter.begin(), filter.end(), filter.begin(), ::tolower);

    for(auto const& account_name : this->loadedWallet_.GetAccountNames()) {
        std::string account_name_lower(account_name);
        std::transform(account_name.begin(), account_name.end(), account_name_lower.begin(), ::tolower);

        if(account_name_lower.find(filter) != std::string::npos) {
            this->ui->listWidget_Accounts->addItem(QString::fromStdString(account_name));
        }
    }
}
