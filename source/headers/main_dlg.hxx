#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <private/qcssparser_p.h>

#include <QDialogButtonBox>
#include <QMainWindow>
#include <QClipboard>
#include <QListWidget>
#include <QMessageBox>
#include <QTreeWidget>
#include <QFileDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QString>
#include <QLabel>
#include <QMenu>
#include <QPair>
#include <QMap>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "crypto.hxx"

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

    // Retains the last path that was used to load a vault.
    QString last_vault_path;

    // Retains the last plain key that was used to decrypt a vault.
    QString last_vault_key;

    // Retains whether or not the last vault loaded was encrypted or not.
    bool last_vault_encrypted;

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

    //
    void saveVault();
    void saveVaultAs();
    void openVault();

    void spawnGenerator();

    void accountSearch(QString);

public:
    bool LoadStylesheet(const std::string&);

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_HH
