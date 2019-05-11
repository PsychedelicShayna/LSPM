#ifndef PASSWORD_GENERATOR_DIALOGUE_HXX
#define PASSWORD_GENERATOR_DIALOGUE_HXX

// Stl Includes
#include <cstdint>

// Qt Includes
#include <QDialog>

// Project Includes
#include "password_generator.hxx"

namespace Ui {
    class PasswordGeneratorDialogue;
}

class PasswordGeneratorDialogue : public QDialog {
private:
    Ui::PasswordGeneratorDialogue* ui_;
    Q_OBJECT

    PasswordGenerator generator_;

    void generateSeed_();

private slots:
    void on_pushButton_Generate_clicked();
    void on_lineEdit_Seed_textEdited(QString const& seed);

    void on_hSlider_Length_valueChanged(int);

    void on_hSlider_AZ_valueChanged(int);
    void on_spinBox_AZ_valueChanged(int);
    void on_hSlider_az_valueChanged(int);
    void on_spinBox_az_valueChanged(int);
    void on_hSlider_09_valueChanged(int);
    void on_spinBox_09_valueChanged(int);
    void on_hSlider_Special_valueChanged(int);
    void on_spinBox_Special_valueChanged(int);
    void on_hSlider_Special2_valueChanged(int);
    void on_spinBox_Special2_valueChanged(int);

public:
    explicit PasswordGeneratorDialogue(QWidget* parent = nullptr);
    ~PasswordGeneratorDialogue();
};

#endif // PASSWORD_GENERATOR_DIALOGUE_HXX
