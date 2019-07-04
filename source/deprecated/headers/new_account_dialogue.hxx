#ifndef NEWENTRY_HH
#define NEWENTRY_HH

#include <QMainWindow>
#include <QMessageBox>
#include <QListWidget>

#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <map>

#include "password_generator_dialogue.hxx"
#include "wallet.hxx"

namespace Ui {
    class NewAccountDialogue;
}

class NewAccountDialogue : public QMainWindow {
protected:
    Ui::NewAccountDialogue* ui;
    Q_OBJECT

    /* This is a function callback that gets called when modifications to
     * the modifiedWalletCopy_ Wallet is complete, this passed the modified
     * Wallet as an argument. This is a required argument in the constructor. */
    std::function<void(Wallet)> modificationsCompleteCallback_;

    /* This is an optional argument passed in the constructor, that stores the name
     * of the account in the Wallet that is being modified that should be deleted/replaced
     * before the new one is inserted. This is only ever used, when the modify button is pressed
     * and not the create button. */
    std::string replacingAccountName_;

    /* This stores a whole copy of the Wallet that will be modified. This is passed
     * as a required argument in the constructor. Modifications will be done on this
     * wallet copy, and then the modificationsCompleteCallback_ std::function will be
     * called when the modifications are complete, which will pass this modified wallet
     * as an argument to the callback. Whatever the callback does is not up to this class, but
     * the intended use, is for the callback to replace the original, with the newly modified copy. */
    Wallet modifiedWalletCopy_;

    QWidget* parent_;

private slots:
    // Event handler for when the password generator action in the menubar is clicked.
    void on_action_Generator_triggered();

    // Event handler for when the insert button is clicked.
    void on_pushButton_Insert_clicked();

    // Event handler for when the remove button is clicked.
    void on_pushButton_Remove_clicked();

    // Event handler for when the done button is clicked.
    void on_pushButton_Done_clicked();

    // Event handler for when an item in the key list widget is double clicked.
    void on_listWidget_Keys_itemDoubleClicked(QListWidgetItem*);

    // Event handler for when an item in the value list wiedget is double clicked.
    void on_listWidget_Values_itemDoubleClicked(QListWidgetItem*);

    void closeEvent(QCloseEvent*) override;

public:
    explicit NewAccountDialogue(const Wallet&, std::function<void(Wallet)>, const std::string*, QWidget* = nullptr);
    ~NewAccountDialogue() override;
};

#endif
