#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

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

#include "password_generator_dialog.hxx"
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

    /* Generates a sequential name, e.g. New File (1), New File (2), based on the supplied blueprint, e.g. New File(%1).
     * The first argument is the blueprint, the second argument is a function used to determine whether or not the
     * generated name is valid, or to continue incrementing. */
    QString generateSequentialName(const QString&, std::function<bool(const QString&)>);

    template<typename T>
    QList<T> collect(int32_t range, std::function<T(int32_t)> accessor) const {
        QList<T> collected;
        for(int32_t i=0; i<range; collected.push_back(accessor(i++)));
        return collected;
    }

    QString last_vault_path;
    QString last_vault_key;
    bool last_vault_encrypted;

private slots:
    QString serializeAccounts() const;
    void deserializeAccounts(const QString&);

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

    void save();
    void saveAs();
    void open();

    void spawnGenerator();

    void on_searchBar_textChanged(QString);

public:
    bool LoadStylesheet(const std::string&);

    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
};

#endif // MAINWINDOW_HH
