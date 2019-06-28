#include "headers/password_generator_dialogue.hxx"
#include "ui_password_generator_dialogue.h"

void PasswordGeneratorDialogue::generateSeed_() {
    this->generator_.Configure({
        {PasswordGenerator::AUPPER, this->ui_->spinBox_AZ->value()},
        {PasswordGenerator::ALOWER, this->ui_->spinBox_az->value()},
        {PasswordGenerator::SPECIAL, this->ui_->spinBox_Special->value()},
        {PasswordGenerator::SEPCIAL2, this->ui_->spinBox_Special2->value()},
        {PasswordGenerator::DECIMAL, this->ui_->spinBox_09->value()}
    });

    this->ui_->lineEdit_Seed->setText(QString::fromStdString(this->generator_.Seed));
}

void PasswordGeneratorDialogue::on_pushButton_Generate_clicked() {
    QString generated_password;
    std::size_t password_length = static_cast<std::size_t>(this->ui_->spinBox_Length->value());
    generated_password = QString::fromStdString(this->generator_.Generate(password_length));
    this->ui_->lineEdit_Password->setText(generated_password);
    this->globalClipboard_->setText(generated_password);
}


void PasswordGeneratorDialogue::on_pushButton_CopyPassword_clicked() {

}

void PasswordGeneratorDialogue::on_lineEdit_Seed_textEdited(const QString& seed) {
    this->generator_.Configure(seed.toStdString());
}

void PasswordGeneratorDialogue::on_hSlider_Length_valueChanged(int value)        { this->ui_->spinBox_Length->setValue(value); }
void PasswordGeneratorDialogue::on_hSlider_AZ_valueChanged(int value)            { this->ui_->spinBox_AZ->setValue(value); }
void PasswordGeneratorDialogue::on_hSlider_az_valueChanged(int value)            { this->ui_->spinBox_az->setValue(value); }
void PasswordGeneratorDialogue::on_hSlider_09_valueChanged(int value)            { this->ui_->spinBox_09->setValue(value); }
void PasswordGeneratorDialogue::on_hSlider_Special_valueChanged(int value)       { this->ui_->spinBox_Special->setValue(value); }
void PasswordGeneratorDialogue::on_hSlider_Special2_valueChanged(int value)      { this->ui_->spinBox_Special2->setValue(value); }
void PasswordGeneratorDialogue::on_spinBox_AZ_valueChanged(int)                  { this->generateSeed_(); }
void PasswordGeneratorDialogue::on_spinBox_az_valueChanged(int)                  { this->generateSeed_(); }
void PasswordGeneratorDialogue::on_spinBox_09_valueChanged(int)                  { this->generateSeed_(); }
void PasswordGeneratorDialogue::on_spinBox_Special_valueChanged(int)             { this->generateSeed_(); }
void PasswordGeneratorDialogue::on_spinBox_Special2_valueChanged(int)            { this->generateSeed_(); }

PasswordGeneratorDialogue::PasswordGeneratorDialogue(QWidget* parent, QClipboard* clipboard) : QDialog(parent), ui_(new Ui::PasswordGeneratorDialogue) {
    ui_->setupUi(this);

    ui_->hSlider_AZ->setValue(1);
    ui_->hSlider_az->setValue(1);
    ui_->hSlider_09->setValue(1);
    ui_->hSlider_Special->setValue(1);
    ui_->hSlider_Special2->setValue(1);
    this->globalClipboard_ = clipboard;
}

PasswordGeneratorDialogue::~PasswordGeneratorDialogue() {
    delete ui_;
}


