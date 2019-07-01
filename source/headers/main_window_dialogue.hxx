#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

/* -------------------------- */
/* Stl Includes               */
/* -------------------------- */
#include <experimental/filesystem>
#include <iostream>
#include <sstream>
#include <iomanip>
/* -------------------------- */

/* -------------------------- */
/* Qt Includes                */
/* -------------------------- */
#include <QMainWindow>
#include <QTextStream>
#include <QListWidget>
#include <QWidget>
#include <QFile>
/* -------------------------- */

/* -------------------------- */
/* Project Form Includes      */
/* -------------------------- */
#include "password_generator_dialogue.hxx"
#include "new_account_dialogue.hxx"
#include "save_file_dialogue.hxx"
#include "load_file_dialogue.hxx"
#include "settings_dialogue.hxx"
/* -------------------------- */

/* -------------------------- */
/* Project Includes           */
/* -------------------------- */
#include "password_generator.hxx"
#include "basic_aes.hxx"
#include "wallet.hxx"
#include "macros.hxx"
/* -------------------------- */

namespace fs = std::experimental::filesystem;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
private:
    Ui::MainWindow* ui;
    Q_OBJECT

    bool walletInMemory_;
    Wallet loadedWallet_;
    std::string walletPath_;
    AesCredentials aesCredentials_;

private slots:
    void on_pushButton_CreateAccount_clicked();
    void on_pushButton_DeleteAccount_clicked();

    void on_action_PasswordGenerator_triggered();

    void on_action_Load_triggered();
    void on_action_Save_triggered();
    void on_action_SaveAs_triggered();
    void on_action_Settings_triggered();

    void on_listWidget_Accounts_itemSelectionChanged();
    void on_listWidget_Keys_itemSelectionChanged();

    void on_listWidget_Accounts_itemDoubleClicked(QListWidgetItem*);

    void on_lineEdit_AccountFilter_textChanged(const QString&);

public:
    const Wallet& LoadedWallet;

    bool LoadQStyleSheet(const std::string&);
    void UpdateListWidgets();

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_HH
