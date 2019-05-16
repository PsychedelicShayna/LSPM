#include "headers/main_window_dialogue.hxx"
#include "headers/new_account_dialogue.hxx"
#include "ui_new_account_dialogue.h"

void NewAccountDialogue::on_action_Generator_triggered() {
    static PasswordGeneratorDialogue* password_generator_dialogue = nullptr;

    if(password_generator_dialogue != nullptr) delete password_generator_dialogue;
    password_generator_dialogue = new PasswordGeneratorDialogue();

    password_generator_dialogue->show();
}

void NewAccountDialogue::on_pushButton_Insert_clicked() {
    QString const& key = this->ui->lineEdit_Key->text();
    QString const& value = this->ui->lineEdit_Value->text();

    if(!(key.size() && value.size())) return;

    if(!this->ui->listWidget_Keys->findItems(key, nullptr).size()) {
        this->ui->listWidget_Keys->addItem(key);
        this->ui->listWidget_Values->addItem(value);
    }

    this->ui->lineEdit_Key->clear();
    this->ui->lineEdit_Value->clear();
}

void NewAccountDialogue::on_pushButton_Remove_clicked() {
    QModelIndexList indexes = this->ui->listWidget_Keys->selectionModel()->selectedIndexes();

    for(QModelIndex const& index : indexes) {
        delete (this->ui->listWidget_Keys->item(index.row()));
        delete (this->ui->listWidget_Values->item(index.row()));
    }
}

void NewAccountDialogue::on_pushButton_Done_clicked() {
    // The account name that was entered in the lineEdit_AccountName line edit.
    const std::string& account_name = this->ui->lineEdit_AccountName->text().toStdString();

    if(!account_name.size()) {
        std::cerr << "Attempted to create an account with no name." << std::endl;

        static QMessageBox* error_message_box = nullptr;
        if(error_message_box != nullptr) delete error_message_box;
        error_message_box = new QMessageBox(this);
        error_message_box->setText("You cannot create an account with no name, please specify a name.");
        error_message_box->show();
        return;
    }

    // If the replacing account name isn't null, then delete the old account first, as the new account is replacing the old one.
    if(this->replacingAccountName_.size()) {
        // A try/catch is set for an invalid argument exception, as an account with that name may not exist.
        try { this->modifiedWalletCopy_.DeleteAccount(this->replacingAccountName_); }
        catch (const std::invalid_argument& exception) {
            std::cerr << exception.what() << std::endl;

            QMessageBox qMessageBox;
            qMessageBox.setText(QString(exception.what()).append(" -- See cerr output for details."));

            std::cerr << "Closing NewAccountDialogue dialogue without making any modifications due to this error." << std::endl;
            this->close();
            return;
        }
    } else if(this->modifiedWalletCopy_.AccountExists(account_name)) {
        QString question = QString("An account with the name \"").append(account_name.c_str()).append("\" already exists, overwrite?");
        if (QMessageBox(QMessageBox::Information, "Overwrite existing account?", question, QMessageBox::Yes | QMessageBox::No).exec() == QMessageBox::Yes) {

        }
    }

    // Checks if an acocunt name with the value of account_name exists, if not, creates it first.
    if(!this->modifiedWalletCopy_.AccountExists(account_name)) {
        this->modifiedWalletCopy_.CreateAccount(account_name);
    }

    // Iterates through each item in the key/value listwidgets, and inserts a pair of both items in the new account.
    for(int32_t index=0; index < this->ui->listWidget_Keys->count(); ++index) {
        QListWidgetItem* key = this->ui->listWidget_Keys->item(index);
        QListWidgetItem* value = this->ui->listWidget_Values->item(index);

        std::string key_string = key->text().toStdString();
        std::string value_string = value->text().toStdString();

        this->modifiedWalletCopy_.SetAccountEntry(account_name, std::make_pair(key_string, value_string));
    }

    // Calls the callback for when the modifications made to the copied wallet are complete.
    this->modificationsCompleteCallback_(this->modifiedWalletCopy_);

    this->close();
}

void NewAccountDialogue::on_listWidget_Keys_itemDoubleClicked(QListWidgetItem* item) {
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    this->ui->listWidget_Keys->editItem(item);
}

void NewAccountDialogue::on_listWidget_Values_itemDoubleClicked(QListWidgetItem* item) {
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    this->ui->listWidget_Values->editItem(item);
}

NewAccountDialogue::NewAccountDialogue(const Wallet& wallet, std::function<void(Wallet)> callback, const std::string* replacing_account, QWidget* parent) : QMainWindow(parent), ui(new Ui::NewAccountDialogue) {
    this->ui->setupUi(this);

    this->setWindowModality(Qt::ApplicationModal);

    this->modifiedWalletCopy_ = wallet;
    this->modificationsCompleteCallback_ = callback;

    if(replacing_account != nullptr) {
        this->replacingAccountName_ = *replacing_account;
        this->ui->lineEdit_AccountName->setText(QString::fromStdString(this->replacingAccountName_));

        for(const auto& pair : this->modifiedWalletCopy_.GetAccountEntries(this->replacingAccountName_)) {
            this->ui->listWidget_Keys->addItem(QString::fromStdString(pair.first));
            this->ui->listWidget_Values->addItem(QString::fromStdString(pair.second));
        }
    }
}

NewAccountDialogue::~NewAccountDialogue() {
    delete ui;
}
