#include "settings_dialogue.hxx"
#include "ui_settings_dialogue.h"

void SettingsDialogue::on_comboBox_EncryptionMode_currentIndexChanged(int index) {
    if(index > 0) this->ui->comboBox_KeyMode->setEnabled(true);
    else this->ui->comboBox_KeyMode->setEnabled(false);
}

void SettingsDialogue::on_toolButton_BrowseStartupWallet_clicked() {
    static QFileDialog* file_dialogue = nullptr;
    if(file_dialogue != nullptr)  delete file_dialogue;

    file_dialogue = new QFileDialog;

    const QString& file_path = file_dialogue->getOpenFileName(this, "Wallet Location", "", "*.uepm");
    this->ui->lineEdit_StartupWalletPath->setText(file_path);
}

void SettingsDialogue::on_toolButton_BrowseStyle_clicked() {
    static QFileDialog* file_dialogue = nullptr;
    if(file_dialogue != nullptr)  delete file_dialogue;

    file_dialogue = new QFileDialog;

    const QString& file_path = file_dialogue->getOpenFileName(this, "Qt Style Sheet", "", "*.qss");
    this->ui->lineEdit_StylePath->setText(file_path);
}

void SettingsDialogue::on_pushButton_SaveSettings_clicked() {
    std::ofstream output_stream("uepm_config.json", std::ios::binary);
    if(output_stream.good()) {
        Json* settings_json = new Json();

        const std::string& style = this->ui->lineEdit_StylePath->text().toStdString();
        const std::string& swpath = this->ui->lineEdit_StartupWalletPath->text().toStdString();
        const std::string& swktype = std::to_string(this->ui->comboBox_KeyMode->currentIndex());
        const std::string& swetype = std::to_string(this->ui->comboBox_EncryptionMode->currentIndex());

        (*settings_json)["style"] = style;
        (*settings_json)["swpath"] = swpath;
        (*settings_json)["swktype"] = swktype;
        (*settings_json)["swetype"] = swetype;

        const std::string json_dump = settings_json->dump();
        output_stream.write(json_dump.c_str(), static_cast<std::streamsize>(json_dump.size()));
        output_stream.close();
        this->callback_(settings_json);
        this->close();
    } else {
        static QMessageBox* messagebox_error = nullptr;
        if(messagebox_error != nullptr) delete messagebox_error;
        messagebox_error = new QMessageBox(this);

        messagebox_error->setText("Cannot open upem_config.json for writing, the stream wasn't good.");
        messagebox_error->show();
    }
}

void SettingsDialogue::closeEvent(QCloseEvent*) {
    this->callback_(nullptr);
}

void SettingsDialogue::reject() {
    this->callback_(nullptr);
}

SettingsDialogue::SettingsDialogue(std::function<void(Json*)> callback, QWidget* parent) : QDialog(parent), ui(new Ui::SettingsDialogue) {
    ui->setupUi(this);

    this->callback_ = callback;

    std::ifstream input_stream("uepm_config.json", std::ios::binary);
    if(input_stream.good()) {
        std::string settings_json_string((std::istreambuf_iterator<char>(input_stream)), (std::istreambuf_iterator<char>()));
        input_stream.close();

        Json settings_json = Json::parse(settings_json_string);

        for(const auto& pair : settings_json.items()) {
            const std::string key = pair.key();
            const std::string value = pair.value();

            if(key == "style") {
                try{ this->ui->lineEdit_StylePath->setText(QString::fromStdString(value)); }
                catch(const std::exception& exception) {
                    std::cerr << exception.what() << std::endl;
                }
            }

            if(key == "swpath") {
                try { this->ui->lineEdit_StartupWalletPath->setText(QString::fromStdString(value)); }
                catch(const std::exception& exception) {
                    std::cerr << exception.what() << std::endl;
                }
            }

            if(key == "swktype") {
                try { this->ui->comboBox_KeyMode->setCurrentIndex(std::stoi(value)); }
                catch(const std::exception& exception) {
                    std::cerr << exception.what() << std::endl;
                }
            }

            if(key == "swetype") {
                try { this->ui->comboBox_EncryptionMode->setCurrentIndex(std::stoi(value)); }
                catch(const std::exception& exception) {
                    std::cerr << exception.what() << std::endl;
                }
            }
        }
    }
}


SettingsDialogue::~SettingsDialogue() {


    delete ui;
}
