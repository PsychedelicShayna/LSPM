#include "headers/main_window_dialogue.hxx"
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
    if(new_account_dialogue != nullptr) delete new_account_dialogue;

    new_account_dialogue = new NewAccountDialogue(this->loadedWallet_, [this](Wallet modifiedWallet) {
        this->loadedWallet_ = modifiedWallet;
        this->UpdateListWidgets();
    }, nullptr, this);

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
    const std::string account_name = item->text().toStdString();

    static NewAccountDialogue* new_account_dialogue = nullptr;
    if(new_account_dialogue != nullptr)  delete new_account_dialogue;

    new_account_dialogue = new NewAccountDialogue(this->loadedWallet_, [this](Wallet wallet) {
        this->loadedWallet_ = wallet;
        this->UpdateListWidgets();
    }, &account_name, this);


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

    load_file_dialogue = new LoadFileDialogue([this](Wallet loadedWallet, std::string walletPath, AesCredentials aesCredentials){
        this->loadedWallet_ = loadedWallet;
        this->aesCredentials_ = aesCredentials;
        this->walletPath_ = walletPath;
        this->walletInMemory_ = true;
        this->UpdateListWidgets();
    }, this);

    load_file_dialogue->show();
}

void MainWindow::on_action_Save_triggered() {
    if(this->walletInMemory_) {
        std::vector<uint8_t> wallet_data = this->loadedWallet_.DumpDirect();

        if(this->aesCredentials_.Valid) {
            BasicAes basic_aes;
            try {
                basic_aes.LoadCredentials(this->aesCredentials_);
                wallet_data = basic_aes.Encrypt(wallet_data);
            } catch(const std::exception& exception) {
                mbalert(exception.what(), this);
                return;
            }
        }

        std::ofstream output_stream(this->walletPath_, std::ios::trunc | std::ios::binary);
        if(output_stream.good()) {
            output_stream.write(reinterpret_cast<char*>(wallet_data.data()), static_cast<std::streamsize>(wallet_data.size()));
            output_stream.close();
            mbalert("The wallet has been saved.", this);
        } else {
            mbalert("The output stream wasn't good.", this);
        }
    } else {
        mbalert("There is no wallet in memory to save, load a wallet first.", this)
    }
}

void MainWindow::on_action_SaveAs_triggered() {
    static SaveFileDialogue* save_file_dialogue = nullptr;
    if(save_file_dialogue != nullptr) delete save_file_dialogue;

    save_file_dialogue = new SaveFileDialogue(this->loadedWallet_, [](){}, this);

    save_file_dialogue->show();
}

void MainWindow::on_action_Settings_triggered() {
    static SettingsDialogue* settings_dialogue = nullptr;
    if(settings_dialogue != nullptr) delete settings_dialogue;

    settings_dialogue = new SettingsDialogue([this](Json json) {
        if(!(this->LoadQStyleSheet((json)["style"]))) {
            std::cerr << "Error when loading stylesheet." << std::endl;
        }
    }, this);

    settings_dialogue->show();
}

void MainWindow::UpdateListWidgets() {
    this->ui->listWidget_Accounts->clear();
    this->ui->listWidget_Keys->clear();
    this->ui->lineEdit_SelectedKeyValue->clear();

    for(auto const& account_name : this->loadedWallet_.GetAccountNames()) {
        this->ui->listWidget_Accounts->addItem(QString::fromStdString(account_name));
    }
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

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow), LoadedWallet(loadedWallet_) {
    ui->setupUi(this);

    this->walletInMemory_ = false;
    this->setWindowIcon(QIcon("./Icon.png"));

    std::ifstream input_stream("lspm_config.json", std::ios::binary);

    if(input_stream.good()) {
        try {
            std::string file_data((std::istreambuf_iterator<char>(input_stream)), (std::istreambuf_iterator<char>()));
            Json json_data = Json::parse(file_data);

            LoadFileDialogue::LoadFileDialogueState lfd_state;

            uint32_t startup_wallet_qualifier = 0;

            for(const auto& pair : json_data.items()) {
                const std::string& setting = pair.key();
                const std::string& value = pair.value();

                if(setting == "swetype") {
                    lfd_state.comboBox_AESMode_Index = std::stoi(value);
                    ++startup_wallet_qualifier;
                }

                if(setting == "swktype") {
                    lfd_state.comboBox_KeyMode_Index = std::stoi(value);
                    ++startup_wallet_qualifier;
                }

                if(setting == "swpath") {
                    if(fs::exists(value)) {
                        lfd_state.lineEdit_FilePath_Text = value;
                        ++startup_wallet_qualifier;
                    }
                }

                if(setting == "style") {
                    if(!(this->LoadQStyleSheet(value))) {
                        std::cerr << "Error when loading stylesheet: " << value << std::endl;
                    }
                }
            }

            if(startup_wallet_qualifier == 3) {
                static LoadFileDialogue* load_file_dialogue = nullptr;
                if(load_file_dialogue != nullptr) delete load_file_dialogue;

                load_file_dialogue = new LoadFileDialogue([this](Wallet loadedWallet, std::string walletPath, AesCredentials aesCredentials) {
                    this->loadedWallet_ = loadedWallet;
                    this->aesCredentials_ = aesCredentials;
                    this->walletPath_ = walletPath;
                    this->walletInMemory_ = true;

                    this->UpdateListWidgets();

                }, this);

                load_file_dialogue->LoadState(lfd_state);
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


