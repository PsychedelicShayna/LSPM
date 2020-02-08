#include "headers/main_dlg.hxx"
#include "ui_main_dlg.h"

bool MainWindow::spawnTextPrompt(const QString& message, QString* output, bool password=false) {
    // The actual dialog object that will prompt the user for text.
    QDialog* dialog = new QDialog(this);

    // Sets a flag that delets the dialog from the heap as soon as it's closed.
    dialog->setAttribute(Qt::WA_DeleteOnClose);

    // Makes the dialog window non-resizable.
    dialog->setWindowFlags(dialog->windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

    // The layout which will contain all of the Ui elements.
    QVBoxLayout* layout = new QVBoxLayout(dialog);

    // Create a new label, lineEdit, and pushBUtton with the dialog as the parent.
    QLabel* message_label = new QLabel(message, dialog); // The message argument is passed, to display the message by default.
    message_label->setFont(QFont("Menlo"));

    QLineEdit* prompt_line_edit = new QLineEdit(dialog);
    prompt_line_edit->setFont(QFont("Menlo"));

    if(password) {
        prompt_line_edit->setEchoMode(QLineEdit::Password);
    }

    QPushButton* okay_button = new QPushButton("Okay", dialog);
    okay_button->setFont(QFont("Menlo"));

    // Add the newly created Ui elements to the verical layout.
    layout->addWidget(message_label);
    layout->addWidget(prompt_line_edit);
    layout->addWidget(okay_button);

    // Assigns the newly constructed layout to the dialog.
    dialog->setLayout(layout);

    /* This will be assigned to the text that was entered, assuming okay was clicked,
     * otherwise it will remain uninitialized. The reason for making a separate
     * variable rather than just reading from the QLineEdit at the end, is because
     * of the flag that was set earlier that deletes the dialog from the heap the
     * moment it is closed, and as the okay button triggers the close event, by the
     * time the dialog is closed, the QLineEdit would have been deleted. */
    QString submitted_text;

    bool submitted = false; // If this remains false, okay was never pressed, the dialog just closed.

    // Connects the "clicked" signal from the okay button, to this lambda.
    QObject::connect(okay_button, &QPushButton::clicked, [&](){
        submitted = true; // Indicates that okay was indeed pressed.
        submitted_text = prompt_line_edit->text(); // Gets the entered text, and stores it in submitted_text.
        dialog->close();
    });

    // Unlike show(), exec() executes synchronously.
    dialog->exec();

    if(submitted) *output = submitted_text;

    return submitted;
}

int32_t MainWindow::randomNumber(const int32_t& from, const int32_t& to) const {
    static std::random_device device;
    static std::mt19937 generator(device());

    std::uniform_int_distribution<int32_t> uniform_distributer(from, to);

    int32_t generated = uniform_distributer(generator);
    return generated;
}

QString MainWindow::generatePassword(const uint32_t& length) const {
    static const QString& uppercase_chars   =   "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static const QString& lowercase_chars   =   "abcdefghijklmnopqrstuvwxyz";
    static const QString& decimal_chars     =   "1234567890";

    QString generation_buffer;
    generation_buffer.reserve(static_cast<int32_t>(length));

    const uint32_t& characters_per_set = static_cast<uint32_t>(std::round(length/3));

    for(uint32_t i=0; i<characters_per_set; ++i) {
        const int32_t& random_index = randomNumber(0, uppercase_chars.size()-1);
        const QChar& random_character = uppercase_chars.at(random_index);
        generation_buffer.push_back(random_character);
    }

    for(uint32_t i=0; i<characters_per_set; ++i) {
        const int32_t& random_index = randomNumber(0, lowercase_chars.size()-1);
        const QChar& random_character = lowercase_chars.at(random_index);
        generation_buffer.push_back(random_character);
    }

    for(uint32_t i=0; i<characters_per_set; ++i) {
        const int32_t& random_index = randomNumber(0, decimal_chars.size()-1);
        const QChar& random_character = decimal_chars.at(random_index);
        generation_buffer.push_back(random_character);
    }

    std::random_shuffle(generation_buffer.begin(), generation_buffer.end());

    return generation_buffer;
}

QString MainWindow::serializeAccounts() const {
    Json json_representation;

    int32_t account_count = ui->accountTree->topLevelItemCount();

    const auto& accounts = collect<QTreeWidgetItem*>(account_count, [this](int32_t i) {
        return ui->accountTree->topLevelItem(i);
    });

    for(const auto& account : accounts) {
        const QString& account_name = account->text(0);
        json_representation[account_name.toStdString()] = {};

        const int32_t entry_count = account->childCount();

        const auto& entries = collect<QTreeWidgetItem*>(entry_count, [&account](int32_t i ){
            return account->child(i);
        });

        for(const auto& entry : entries) {
            const QString& entry_name = entry->text(0);
            const QString& entry_value = entry->text(1);

            json_representation[account_name.toStdString()][entry_name.toStdString()] = entry_value.toStdString();
        }
    }

    return QString::fromStdString(json_representation.dump(1, ' '));
}
void MainWindow::deserializeAccounts(const QString& serialized_json) {
    const Json& json = Json::parse(serialized_json.toStdString());

    ui->accountTree->clear();

    for(const auto& account_object : json.items()) {
        QTreeWidgetItem* account = new QTreeWidgetItem;
        account->setText(0, QString::fromStdString(account_object.key()));

        for(const auto& entry_object : account_object.value().items()) {
            QTreeWidgetItem* entry = new QTreeWidgetItem;
            entry->setText(0, QString::fromStdString(entry_object.key()));
            entry->setText(1, QString::fromStdString(entry_object.value()));
            account->addChild(entry);
        }

        ui->accountTree->addTopLevelItem(account);
    }
}

void MainWindow::showAccountTreeContextMenu(const QPoint& relative_click_point) {
    const QPoint& global_click_point = ui->accountTree->mapToGlobal(relative_click_point);

    QMenu context_menu(this);
    context_menu.setFont(QFont("Menlo"));

    context_menu.addAction("Create account", this, SLOT(createAccount()));
    context_menu.addSeparator();

    if(ui->accountTree->selectedItems().size() == 1) {
        QTreeWidgetItem* selected_item = ui->accountTree->selectedItems().at(0);

        // Top level items (accounts) have no parent set, while child items (entries) do.
        if(selected_item->parent() == nullptr) {
            context_menu.addAction("Rename account", this, SLOT(renameAccount()));
            context_menu.addAction("Delete account", this, SLOT(deleteAccount()));
            context_menu.addSeparator();
            context_menu.addAction("Create entry", this, SLOT(createEntry()));
        } else {
            context_menu.addAction("Rename entry", this, SLOT(renameEntry()));
            context_menu.addAction("Delete entry", this, SLOT(deleteEntry()));

            context_menu.addSeparator();

            context_menu.addAction("Set value", this, SLOT(setEntryValue()));
            context_menu.addAction("Copy value", this, SLOT(copyEntryValue()));

            context_menu.addSeparator();

            QMenu* generator_menu = context_menu.addMenu("Generate Password");

            generator_menu->addAction("12 Characters", [this, &selected_item](){
                selected_item->setText(1, generatePassword(12));
            });

            generator_menu->addAction("24 Characters", this, [this, &selected_item](){
                selected_item->setText(1, generatePassword(24));
            });

            generator_menu->addAction("48 Characters", this, [this, &selected_item](){
                selected_item->setText(1, generatePassword(48));
            });

            generator_menu->addAction("96 Characters", this, [this, &selected_item](){
                selected_item->setText(1, generatePassword(96));
            });

            generator_menu->addAction("192 Characters", this, [this, &selected_item](){
                selected_item->setText(1, generatePassword(192));
            });

            generator_menu->addAction("384 Characters", this, [this, &selected_item](){
                selected_item->setText(1, generatePassword(384));
            });
        }
    } else if (ui->accountTree->selectedItems().size() > 1) {
        const auto& selected_items = ui->accountTree->selectedItems();

        if(std::all_of(selected_items.begin(), selected_items.end(), [](QTreeWidgetItem* item){return item->parent();})) {
            context_menu.addAction("Delete entries", this, SLOT(deleteEntry()));
        } else if (std::none_of(selected_items.begin(), selected_items.end(), [](QTreeWidgetItem* item){return item->parent();})) {
            context_menu.addAction("Delete accounts", this, SLOT(deleteAccount()));
        }
    }

    context_menu.exec(global_click_point);
}

void MainWindow::createAccount() {
    QString new_account_name;

    for(;;) {
        bool submitted = spawnTextPrompt("Enter the name for the new account.", &new_account_name);

        if(submitted) {
            const auto& matches = ui->accountTree->findItems(new_account_name, Qt::MatchExactly);

            bool duplictae = std::any_of(matches.begin(), matches.end(), [&](QTreeWidgetItem* item){
                return !item->parent();
            });

            if(duplictae) {
                QMessageBox::warning(this, "Error!", "An account with that name already exists.");
            } else {
                break;
            }
        } else {
            return;
        }
    }

    QTreeWidgetItem* new_account= new QTreeWidgetItem;
    new_account->setText(0, new_account_name);
    ui->accountTree->addTopLevelItem(new_account);
}
void MainWindow::deleteAccount() {
    for(const auto& item : ui->accountTree->selectedItems()) {
        delete item;
    }
}
void MainWindow::renameAccount() {
    QTreeWidgetItem* selected_account = ui->accountTree->selectedItems().at(0);
    QString new_account_name;

    while(true) {
        bool submitted = spawnTextPrompt("Enter the new name of the account.", &new_account_name);

        if(submitted) {
            bool duplicate = ui->accountTree->findItems(new_account_name, Qt::MatchExactly).size();

            if(duplicate) {
                QMessageBox::warning(this, "Error!", "An account with that name already exists.");
            } else {
                selected_account->setText(0, new_account_name);
                break;
            }
        } else {
            break;
        }
    }
}

void MainWindow::createEntry() {
    QTreeWidgetItem* selected_account = ui->accountTree->selectedItems().at(0);

    QString new_entry_name;

    while(true) {
        bool submitted = spawnTextPrompt("Enter the name of the new entry.", &new_entry_name);

        if(submitted) {
            const auto& children = collect<QTreeWidgetItem*>(selected_account->childCount(), [&selected_account](int32_t i) {
                return selected_account->child(i);
            });

            bool duplicate = std::any_of(children.begin(), children.end(), [&new_entry_name](QTreeWidgetItem* item){
                return item->text(0) == new_entry_name;
            });

            if(duplicate) {
                 QMessageBox::warning(this, "Error!", "An entry with that name already exists.");
            } else {
                QTreeWidgetItem* new_entry = new QTreeWidgetItem;
                new_entry->setText(0, new_entry_name);
                selected_account->addChild(new_entry);
                break;
            }
        } else {
            break;
        }
    }
}
void MainWindow::deleteEntry() {
    for(const auto& item : ui->accountTree->selectedItems()) {
        if(item->parent()) delete item;
    }
}
void MainWindow::renameEntry() {
    QTreeWidgetItem* selected_entry = ui->accountTree->selectedItems().at(0);

    const QTreeWidgetItem* parent = selected_entry->parent();

    if(selected_entry->parent()) {
        const auto& children = collect<QTreeWidgetItem*>(parent->childCount(), [&parent](int32_t i) {
            return parent->child(i);
        });

        QString new_entry_name;

        while(true) {
            bool submitted = spawnTextPrompt("Enter the new name of the entry.", &new_entry_name);
            if(submitted) {
                bool duplicate = std::any_of(children.begin(), children.end(), [&new_entry_name](QTreeWidgetItem* child){
                    return child->text(0) == new_entry_name;
                });

                if(duplicate) {
                    QMessageBox::warning(this, "Error!", "An entry with that name already exists.");
                } else {
                    selected_entry->setText(0, new_entry_name);
                    break;
                }
            } else {
                break;
            }
        }
    }
}
void MainWindow::setEntryValue() {
    QTreeWidgetItem* selected_entry = ui->accountTree->selectedItems().at(0);

    if(selected_entry->parent()) {
        selected_entry->setFlags(selected_entry->flags() | Qt::ItemIsEditable);
        ui->accountTree->editItem(selected_entry, 1);
        selected_entry->setFlags(selected_entry->flags() ^ Qt::ItemIsEditable);
    }
}
void MainWindow::copyEntryValue() {
    const QTreeWidgetItem* selected_entry = ui->accountTree->selectedItems().at(0);
    const QString& selected_entry_value = selected_entry->text(1);

    QClipboard* global_clipboard = QApplication::clipboard();
    global_clipboard->setText(selected_entry_value);
}

void MainWindow::on_accountTree_itemDoubleClicked(QTreeWidgetItem* item, int column) {
    if(item->parent() && column == 1) {
        item->setFlags(item->flags() | Qt::ItemIsEditable);
        ui->accountTree->editItem(item, 1);
        item->setFlags(item->flags() ^ Qt::ItemIsEditable);
    } else if(item->parent()) {
        renameEntry();
    } else {
        renameAccount();
    }
}

void MainWindow::saveVault() {
    const std::string& output_path = lastVaultPath.toStdString();

    std::ofstream output_stream(output_path, std::ios::binary);
    if(output_stream.good()) {
        const std::string& serialized_string = serializeAccounts().toStdString();
        std::vector<uint8_t> bytes_to_write(serialized_string.begin(), serialized_string.end());

        if(lastVaultEncrypted) {
            bytes_to_write = Crypto::Aes256CbcAutoEncrypt(bytes_to_write, lastVaultKey.toStdString());
        }

        output_stream.write(reinterpret_cast<char*>(bytes_to_write.data()), static_cast<std::streamsize>(bytes_to_write.size()));
        output_stream.close();

        QMessageBox::information(this, "Vault saved", lastVaultEncrypted ? "The vault has been encrypted and saved." : "The vault has been saved.");
    } else {
        QMessageBox::critical(this, "Error!", "There was an error opening the file. The stream reported bad.");
    }
}
void MainWindow::saveVaultAs() {
    const std::string& serialized_string = serializeAccounts().toStdString();

    std::vector<uint8_t> bytes_to_write(serialized_string.begin(), serialized_string.end());

    while(true) {
        if(QMessageBox::question(this, "Save Vault", "Use AES-256 encryption?", QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes) {
            QString submitted_text;

            if(spawnTextPrompt("Enter the password you want to use to encrypt this vault.", &submitted_text)) {
                const std::string& plain_key = submitted_text.toStdString();
                bytes_to_write = Crypto::Aes256CbcAutoEncrypt(bytes_to_write, plain_key);
                break;
            }
        } else {
            break;
        }
    }

    const std::string& save_path = QFileDialog::getSaveFileName(this, "Save Vault", "", "Vaults (*.vlt *.vault)").toStdString();

    std::ofstream output_stream(save_path, std::ios::binary);

    if(output_stream.good()) {
        output_stream.write(reinterpret_cast<const char*>(bytes_to_write.data()), static_cast<std::streamsize>(bytes_to_write.size()));
        output_stream.close();
    }
}

void MainWindow::parseVault(const QString& load_path) {
    std::ifstream input_stream(load_path.toStdString(), std::ios::binary);
    if(input_stream.good()) {
        // Copy is made, in case decryption fails. Copy of the original bytes is needed to re-attempt decryption.
        std::vector<uint8_t> original_bytes_copy((std::istreambuf_iterator<char>(input_stream)), (std::istreambuf_iterator<char>()));

        // Bytes that will actually be loadeed, assigned to decrypted original bytes if parsing error occurs.
        std::vector<uint8_t> target_bytes(original_bytes_copy);

        input_stream.close();

        ui->saveButton->setEnabled(false);
        lastVaultEncrypted = false;

        while(true) {
            try {
                const std::string json_data(target_bytes.begin(), target_bytes.end());
                Json::parse(json_data);

                deserializeAccounts(QString::fromStdString(json_data));

                lastVaultPath = load_path;
                ui->saveButton->setEnabled(true);
                break;
            } catch(const Json::exception&) {
                QString submitted_text;

                if(spawnTextPrompt("Parsing exception. The vault is probably encrypted (or corrupted). Please enter your encryption key.", &submitted_text, true)) {
                    const std::string& plain_key = submitted_text.toStdString();
                    target_bytes = Crypto::Aes256CbcAutoDecrypt(original_bytes_copy, plain_key);

                    lastVaultKey = QString::fromStdString(plain_key);
                    lastVaultEncrypted = true;
                } else {
                    break;
                }
            }
        }
    } else {
        QMessageBox::critical(this, "Error!", "There was an error opening the file. The stream reported bad.");
    }
}
void MainWindow::openVault() {
    const QString& load_path = QFileDialog::getOpenFileName(this, "Load Vault", "", "Vaults (*.vlt *.vault)");
    parseVault(load_path);
}

void MainWindow::spawnGenerator() {
    PasswordGeneratorDialog* password_generator_dialog = new PasswordGeneratorDialog(this);
    password_generator_dialog->show();
}

void MainWindow::accountSearch(QString new_text) {
    const int32_t item_count = ui->accountTree->topLevelItemCount();

    const auto& accounts = collect<QTreeWidgetItem*>(item_count, [this](int32_t i){
        return ui->accountTree->topLevelItem(i);
    });

    if(new_text.size()) {
        for(const auto& account : accounts) {
            if(!(account->text(0).contains(new_text, Qt::CaseInsensitive))) {
                account->setHidden(true);
            } else {
                account->setHidden(false);
            }
        }
    } else {
        for(const auto& account : accounts) {
            account->setHidden(false);
        }
    }
}

bool MainWindow::LoadStylesheet(const std::string& qstylesheet_path) {
    std::ifstream input_stream(qstylesheet_path, std::ios::binary);

    if(input_stream.good()) {
        // Constructs a QString from an std::string that is being constructed with a start/end iterator of the input stream.
        const QString& qss_string = QString::fromStdString(std::string(
            (std::istreambuf_iterator<char>(input_stream)),
            (std::istreambuf_iterator<char>())
        ));

        /* QCss::Parser/StyleSheet are both part of the private QtGui and QtCore libraries.
         * There is no official public way to check the validity of a Qt StyleSheet. */
        QCss::Parser qss_parser(qss_string);
        QCss::StyleSheet stylesheet;

        // If the parser returns true, the stylesheet is valid, and is applied, otherwise an error message is shown.
        if(qss_parser.parse(&stylesheet)) {
            setStyleSheet(qss_string);
            return true;
        } else {
            QMessageBox::warning(this, "QStyleSheet parsing failure.",
                "There was an error when trying to parse style.qss, verify that the QSS is valid, or delete the file to disable styling.");
            return false;
        }
    } else {
        return false;
    }
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    QScreen* primary_screen = QGuiApplication::primaryScreen();
    QRect primary_screen_geometry = primary_screen->geometry();

    resize(primary_screen_geometry.width()/2, primary_screen_geometry.height()/2);

    // Attempt to load and apply the main configuration file.
    std::ifstream input_stream("./config.json", std::ios::binary);

    if(input_stream.good()) {
        Json config_json;
        input_stream >> config_json;
        input_stream.close();

        for(auto& pair : config_json.items()) {
            if(pair.key() == "qstylesheet") {
                LoadStylesheet(pair.value());
            } else if(pair.key() == "vault") {
                parseVault(QString::fromStdString(pair.value()));
            }
        }
    }

    // Disables the save button by default, until it's re-enabled by the open method after a vault has been loaded.
    ui->saveButton->setEnabled(false);

    // Makes sure that there's a starting distance of 200 between the name/value columns in the account tree.
    ui->accountTree->setColumnWidth(0, 200);

    // Connects the context menu requested signal from the account tree, to the show context menu slot.
    connect(ui->accountTree, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showAccountTreeContextMenu(const QPoint&)));

    // Connects the text changed signal from the search bar, to the account search slot.
    connect(ui->searchBar, SIGNAL(textChanged(QString)), this, SLOT(accountSearch(QString)));

    // Connects the main top buttons to their respective slots.
    connect(ui->generatorButton, SIGNAL(clicked()), this, SLOT(spawnGenerator()));
    connect(ui->saveAsButton, SIGNAL(clicked()), this, SLOT(saveVaultAs()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveVault()));
    connect(ui->openButton, SIGNAL(clicked()), this, SLOT(openVault()));
}

MainWindow::~MainWindow() {
    delete ui;
}
