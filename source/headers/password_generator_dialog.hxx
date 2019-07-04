#ifndef MAIN_WINDOW_HXX
#define MAIN_WINDOW_HXX

// Qt's Private QSS parser.
#include <QtGui/private/qcssparser_p.h>

// Qt Elements.
#include <QMainWindow>
#include <QMessageBox>
#include <QClipboard>
#include <QCheckBox>
#include <QSpinBox>

// Std algorithms.
#include <algorithm>
#include <random>

// Std I/O.
#include <iostream>
#include <fstream>

// Std containers.
#include <string>
#include <vector>

namespace Ui {
    class PasswordGeneratorDialog;
}

class PasswordGeneratorDialog : public QMainWindow {
private:
    Q_OBJECT
    Ui::PasswordGeneratorDialog* ui;

    // Constants of each character set, used several times throughout the code to generate passwords.
    static constexpr const char* upperCase_ = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static constexpr const char* lowerCase_ = "abcdefghijklmnopqrstuvwxyz";
    static constexpr const char* special_   = "!@#$%^&*()_+-=[]{}/,.";
    static constexpr const char* numerical_ = "1234567890";

    // A list of spinbox/checkbox pointer pairs, used to iterate them easily, as they share related values.
    std::vector<std::pair<QSpinBox*, QCheckBox*>> charsetSelections_;

    // A simple wrapper around the standard Mersenne Twister implementation to simplify random number generation.
    int32_t randomNumber_(const int32_t&, const int32_t&) const;

private slots:
    /* Sets the LCD to the currently selected password size.
     * This is under the slots section, as signals from other elements,
     * such as spinboxes and checkboxes are connected to this slot, so that
     * the LCD updates every time there is a change in character set/count. */
    void setLcdToSum_();

    // Generates a password using the enabled character sets and their occurance counters.
    void on_btn_generate_clicked();

    // Distributes the selected password size accross the selected character sets.
    void on_btn_distribute_clicked();

    // Generates a password using the supplied seed, and writes the result to the output textEdit.
    void on_btn_genBySeed_clicked();

    // Simply copies the generated password to the clipboard.
    void on_btn_copy_clicked();

    // If the password size for seed based generation changes, this changes the LCD to reflect that.
    void on_spin_sizeBySeed_valueChanged(int);

public:
    explicit PasswordGeneratorDialog(QWidget* parent = nullptr);
    ~PasswordGeneratorDialog();
};

#endif // MAIN_WINDOW_HXX
