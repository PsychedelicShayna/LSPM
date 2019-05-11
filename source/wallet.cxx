#include "wallet.hxx"

void Wallet::LoadDirect(std::vector<uint8_t> data, std::vector<uint8_t>* key) {
    if(key != nullptr) data = this->DecryptionFunction(data, *key);

    Json jWallet;

    try{ jWallet = Json::parse(std::string(data.begin(), data.end())); }
    catch(const Json::exception&) {
        throw std::invalid_argument("Failed to parse; the data is probably encrypted, but decryption is disabled.");
    }

    this->walletData_.clear();

    for(auto const& account : jWallet.items()) {
        auto const& account_name = account.key();
        auto const& account_entries = account.value();

        std::map<std::string, std::string> account_entries_buffer;

        for(auto const& entry : account_entries.items()) {
            auto const& entry_key = entry.key();
            auto const& entry_value = entry.value();

            account_entries_buffer.insert(std::make_pair(entry_key, entry_value.get<std::string>()));
        }

        this->walletData_.insert(std::make_pair(account_name, account_entries_buffer));
    }
}

void Wallet::LoadFile(const std::string& path, std::vector<uint8_t>* key) {
    std::ifstream input_stream(path, std::ios::binary);
    if(!input_stream.good()) {
        std::string error_message = std::string("Attempted to open the file").append(path).append(" but the stream wasn't good. The file may not exist.");
        throw std::invalid_argument(error_message);
    }

    std::istreambuf_iterator<char> iterator_start(input_stream);
    std::istreambuf_iterator<char> iterator_end;

    std::vector<uint8_t> data(iterator_start, iterator_end);

    input_stream.close();

    this->LoadDirect(data, key);
}

std::vector<uint8_t> Wallet::DumpDirect(const std::vector<uint8_t>* key) const {
    std::string json_dump = Json(this->walletData_).dump();
    std::vector<uint8_t> return_buffer(json_dump.begin(), json_dump.end());

    if(key != nullptr) return_buffer = this->EncryptionFunction(return_buffer, *key);
    return return_buffer;
}

void Wallet::DumpFile(const std::string& path, const std::vector<uint8_t>* key) const {
    std::vector<uint8_t> data = this->DumpDirect(key);

    std::ofstream output_stream(path, std::ios::binary | std::ios::trunc);
    if(output_stream.good()) {
        output_stream.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size()));
        output_stream.close();
    } else {
        throw std::invalid_argument(std::string("Could not open the file: ").append(path));
    }
}

void Wallet::SetAccount(const std::string& accountName, const std::map<std::string, std::string>& entryMap) {
    this->walletData_.insert_or_assign(accountName, entryMap);
}

void Wallet::SetAccountEntry(const std::string& accountName, std::pair<std::string, std::string> entryPair)  {
    if(this->walletData_.count(accountName)) {
        this->walletData_.at(accountName).insert_or_assign(entryPair.first, entryPair.second);
    } else {
        const std::string error_message = std::string("Wallet::SetAccount -- Attempted to set account entries of the account ").append(accountName).append(" but the count is zero, so it probably doesn't exist.\n");
        throw std::invalid_argument(error_message);
    }
}

void Wallet::CreateAccount(const std::string& accountName) {
    this->walletData_.insert_or_assign(accountName, std::map<std::string, std::string>());
}

bool Wallet::AccountExists(const std::string& accountName) const {
    return this->walletData_.count(accountName);
}

void Wallet::DeleteAccount(const std::string& name) {
    std::map<std::string, std::map<std::string, std::string>>::iterator iterator;
    iterator = this->walletData_.find(name);

    if(iterator == this->walletData_.end()) {
        const std::string error_message = std::string("Wallet::DeleteAccount -- Attempted to delete account ").append(name).append(" but it the find iterator reached the end, so it probably doesn't exist.\n");
        throw std::invalid_argument(error_message);
    } else {
        this->walletData_.erase(iterator);
    }
}

std::vector<std::string> Wallet::GetAccountNames() const {
    std::vector<std::string> return_buffer;

    for (const auto& pair : this->walletData_) {
        return_buffer.push_back(pair.first);
    }

    return return_buffer;
}

std::vector<std::pair<std::string, std::string>> Wallet::GetAccountEntries(const std::string& accountName) const {
    if(!this->walletData_.count(accountName)) {
        std::string error_message = std::string("Wallet::GetAccountEntries -- Attempted to get entries of the account ").append(accountName).append(" but the count is zero, so it probably doesn't exist.\n");
        throw std::invalid_argument(error_message);
    }

    std::vector<std::pair<std::string, std::string>> return_buffer;

    for(const auto& pair : this->walletData_.at(accountName)) {
        return_buffer.emplace_back(pair);
    }

    return return_buffer;
}


void Wallet::operator=(const Wallet& old)  {
    this->walletData_ = old.walletData_;
    this->EncryptionFunction = old.EncryptionFunction;
    this->DecryptionFunction = old.DecryptionFunction;
}

Wallet::Wallet(const Wallet& old) : WalletData(walletData_) {
    *this = old;
}

Wallet::Wallet() : WalletData(walletData_) {
    this->EncryptionFunction = [](std::vector<uint8_t> data, std::vector<uint8_t> key) -> std::vector<uint8_t> {
        BasicAes basicAes;
        basicAes.LoadKey<BasicAes::AES_256>(key);
        return basicAes.Encrypt(data);
    };

    this->DecryptionFunction = [](std::vector<uint8_t> data, std::vector<uint8_t> key) -> std::vector<uint8_t> {
        BasicAes basicAes;
        basicAes.LoadKey<BasicAes::AES_256>(key);
        return basicAes.Decrypt(data);
    };
}

Wallet::Wallet(CryptoFunction encryptionFunction, CryptoFunction decryptionFunction)  : WalletData(walletData_) {
    this->EncryptionFunction = encryptionFunction;
    this->DecryptionFunction = decryptionFunction;
}
