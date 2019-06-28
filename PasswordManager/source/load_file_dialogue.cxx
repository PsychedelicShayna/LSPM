#include "headers/load_file_dialogue.hxx"
#include "ui_load_file_dialogue.h"

void LoadFileDialogue::on_toolButton_Browse_clicked() {
    static QFileDialog* file_dialogue = nullptr;
    if(file_dialogue != nullptr) delete file_dialogue;
    file_dialogue = new QFileDialog;

    const QString& file_path = file_dialogue->getOpenFileName(this, "Wallet Location", "", "*.lspm");
    this->ui->lineEdit_FilePath->setText(file_path);
}

void LoadFileDialogue::on_comboBox_AESMode_currentIndexChanged(int index) {
    this->ui->lineEdit_EncryptionKey->setEnabled(index != 0);
    this->ui->comboBox_KeyMode->setEnabled(index != 0);
}

// Event handler for when the LoadFile button is clicked.
void LoadFileDialogue::on_pushButton_LoadFile_clicked() {

    // Fetches the path currently typed into the FilePath lineEdit box.
    const std::string file_path = ui->lineEdit_FilePath->text().toStdString();

    // Opens an input stream to the file at that path.
    std::ifstream input_stream(file_path, std::ios::binary);

    // If the stream isn't good, then the whole operation is aborted, and nothing is done.
    if(input_stream.good()) {
        /* Reads the entire contents of the input_stream into the file_data vector, and closes the stream.
         * At this point, the file data should either contain AES encrypted JSON data, or raw JSON data.
         * The JSON data will later be parsed into a wallet object (if encrypted, decryption is done first). */
        std::vector<uint8_t> file_data((std::istreambuf_iterator<char>(input_stream)), (std::istreambuf_iterator<char>()));
        input_stream.close();

        // Will store the parsed wallet content (if parsing is successful).
        Wallet loaded_wallet;

        // Will store the AesCredentials used to decrypt the wallet (if applicable).
        AesCredentials aes_credentials;

        /* If the current index of the AESMode comboBox is greater than zero, then something other than "No Encryption"
         * is currently selected, as "No Encryption" is the first element, therefore index 0, and the rest of the elements
         * are various AES encryption modes (128, 192, 256). If this is true, then encryption is enabled, and the following
         * code will be responsible for decrypting the file data before attempting to parse it. */
        if(ui->comboBox_AESMode->currentIndex() > 0) {
            BasicAes basic_aes;

            // Converts the current AESMode comboBox index (1, 2, or 3) into an AES_MODE enum with a value of either 16, 24, or 32.
            AES_MODE aes_mode = static_cast<AES_MODE>((8*ui->comboBox_AESMode->currentIndex()) + 8);

            // Reads the encryption key from lineEdit_EncryptionKey, and converts it into a byte vector.
            std::string encryption_key_string = ui->lineEdit_EncryptionKey->text().toStdString();
            std::vector<uint8_t> encryption_key(encryption_key_string.begin(), encryption_key_string.end());

            try {
                // If the current index of the KeyMode comboBox is zero (digest mode) then load the Sha256 digest of the encryption key.
                if(ui->comboBox_KeyMode->currentIndex() == 0) basic_aes.LoadKeySha256(encryption_key, aes_mode);

                /* If the current index of the KeyMode comboBox is one (direct mode) then load the encryption key directly.
                 * This throw an exception if the encryption key doesn't match the mode key size. */
                else if(ui->comboBox_KeyMode->currentIndex() == 1) basic_aes.LoadKey(encryption_key, aes_mode);

                // Now that the key has been loaded, the file data is re-assigned to the decrypted version of itself.
                file_data = basic_aes.Decrypt(file_data);

                // An AesCredentials structure is dumped from the basic_aes object, storing the encryption/decryption key, and the iv currently loaded.
                aes_credentials = basic_aes.DumpCredentials();
            }

            // The most common points of failure in the try block throw invalid_argument exceptions.
            catch (const std::invalid_argument& exception) {
                mbalert(exception.what(), this)
                std::cerr << exception.what() << std::endl;
                return;
            }
        }

        try {
            // At this point, if decryption was enabled, file_data has been decrypted, and is ready to be parsed. This parses it into loadd_wallet.
            loaded_wallet.LoadDirect(file_data);
        } catch(const std::invalid_argument& exception) {
            mbalert(exception.what(), this)
            return;
        }


        /* Calls the dialogue callback with the newly parsed wallet data, the path to the file that was read, and the AesCredentials used to
         * decrypt the file, if applicable. If aes_credentials was dumped from loaded_wallet, then the aes_credentials.Valid will be true,
         * otherwise, if it is left initialized via the default constructor, then it will be false. */
        this->walletLoadedCallback_(loaded_wallet, file_path, aes_credentials);
        this->close();
    } else {
       mbalert("The input stream to the file wasn't good.", this)
    }
}

void LoadFileDialogue::on_lineEdit_FilePath_textChanged(const QString& pathqs) {
    const std::string& path = pathqs.toStdString();

    this->ui->lineEdit_FilePath->setStyleSheet(fs::exists(path) && !fs::is_directory(path) ? "color:#00FF00" : "color:#FF0000");
}
void LoadFileDialogue::LoadState(LoadFileDialogueState state) {
    ui->lineEdit_FilePath->setText(QString::fromStdString(state.lineEdit_FilePath_Text));
    ui->comboBox_AESMode->setCurrentIndex(state.comboBox_AESMode_Index);
    ui->comboBox_KeyMode->setCurrentIndex(state.comboBox_KeyMode_Index);
}

void LoadFileDialogue::closeEvent(QCloseEvent*) {
    this->parent_->setEnabled(true);
}

void LoadFileDialogue::reject() {
    this->parent_->setEnabled(true);
}

LoadFileDialogue::LoadFileDialogue(std::function<void(Wallet, std::string, AesCredentials)> callback, QWidget* parent) : QDialog(parent), ui(new Ui::LoadFileDialogue) {
    this->walletLoadedCallback_ = callback;
    this->parent_ = parent;

    parent->setEnabled(false);
    this->setEnabled(true);

    ui->setupUi(this);
}

LoadFileDialogue::~LoadFileDialogue() {
    delete ui;
}




