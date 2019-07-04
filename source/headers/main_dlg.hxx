#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

/* This is Qt's own private QSS parser. The reason this is included
 * is because there is no native way using Qt's public API to validate
 * a Qt Stylesheet. The private QSS parser contains a method that can
 * be used to validate QSS. This is used by the LoadStylesheet method. */
#include <private/qcssparser_p.h>

// Qt GUI Widgets.
#include <QMainWindow>
#include <QListWidget>
#include <QMessageBox>
#include <QTreeWidget>
#include <QFileDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QLabel>
#include <QMenu>

// Qt datatypes.
#include <QClipboard>
#include <QString>

// Standard I/O.
#include <iostream>
#include <fstream>

// Standard datatypes.
#include <algorithm>
#include <string>
#include <vector>

// Crypto namespace, for basic cryptographic functions.
#include "crypto.hxx"

// Password generator dialog. This is ripped from my other repository: https://github.com/Shayna-x02/Password-Generator
#include "generator_dlg.hxx"

// Nlohmann's open source JSON library.
#include "json.hxx"
using Json = nlohmann::json;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
private:
    Ui::MainWindow* ui;
    Q_OBJECT

    /* Simple function to spawn a QDialog, prompting for text.
     * The first argument sets the message that is shown.
     * The second is a pointer to a QString where the output text
     * will be written to. The return value indicates whether
     * or not the input was confirmed (true) or if the dialog
     * was simply closed (false). */
    bool spawnTextPrompt(const QString&, QString*);

    /* Collects a range of variables from a container that exposes no
     * iterator, but does expose its element count, and a way to access
     * a specific element, at a specific index.
     *
     * Here is an example with a vector (though you don't need to do this with a vector because it exposes iterators).
     *
     * const std::vector<int>& vector = {1, 2, 3, 4, 5};
     * const QList<int>& collected = collect<int>(vector.size(), [&](int32_t i){return vector[i];});
     */
    template<typename T>
    QList<T> collect(int32_t range, std::function<T(int32_t)> accessor) const {
        QList<T> collected;
        for(int32_t i=0; i<range; collected.push_back(accessor(i++)));
        return collected;
    }

    // A simple wrapper around the standard Mersenne Twister implementation to simplify random number generation.
    int32_t randomNumber(const int32_t&, const int32_t&) const;

    // Generates a password composed of uppercase, lowercase, and decimal characters with the specified length.
    QString generatePassword(const uint32_t&) const;

    // Retains the last path that was used to load a vault.
    QString lastVaultPath;

    // Retains the last plain key that was used to decrypt a vault.
    QString lastVaultKey;

    // Retains whether or not the last vault loaded was encrypted or not.
    bool lastVaultEncrypted;


private slots:
    /* Serializes the accountTree object into JSON, and
     * deserializes JSON into the accountTree object.
     *
     * This is used to load vault files, assuming they have
     * already been decrypted, as vault files are simply JSON
     * that represents the accountTree object. */
    QString serializeAccounts() const;
    void deserializeAccounts(const QString&);

    // Slot that connects to the customContextMenuRequested signal from the account tree.
    void showAccountTreeContextMenu(const QPoint&);

    // Account C.R.U.D
    void createAccount();
    void deleteAccount();
    void renameAccount();

    // Entry C.R.U.D
    void createEntry();
    void deleteEntry();
    void renameEntry();
    void setEntryValue();
    void copyEntryValue();

    void on_accountTree_itemDoubleClicked(QTreeWidgetItem*, int);

    // Slots for the main navigation buttons.
    void saveVault();
    void saveVaultAs();
    void openVault();
    void spawnGenerator();

    /* Account search slot. Performs a search with the given QString.
     * textChanged Signal from the search box is connected to this. */
    void accountSearch(QString);

public:
    // Attempts to load a stylesheet file, returns true if successful.
    bool LoadStylesheet(const std::string&);

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_HH
