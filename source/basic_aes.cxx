#include "headers/basic_aes.hxx"

void AesCredentials::operator=(const AesCredentials& old) {
    this->InitializationVector = old.InitializationVector;
    this->EncryptionKey = old.EncryptionKey;
    this->DecryptionKey = old.DecryptionKey;
    this->Valid = old.Valid;
}

AesCredentials::AesCredentials() : Valid(false) {}

AesCredentials::AesCredentials(std::array<uint8_t, AES_BLOCK_SIZE> iv, AES_KEY e_key, AES_KEY d_key) : Valid(true) {
    this->InitializationVector = iv;
    this->EncryptionKey = e_key;
    this->DecryptionKey = d_key;
}

std::vector<uint8_t> BasicAes::ApplyPkcs7(std::vector<uint8_t> data, const uint8_t multiple) {
    uint8_t difference = multiple - (data.size() % multiple);
    if(difference == multiple) return data;

    data.resize(data.size()+difference, difference);
    return data;
}

std::vector<uint8_t> BasicAes::RemovePkcs7(std::vector<uint8_t> data) {
    const uint8_t& last_byte = data.at(data.size()-1);

    const auto validator_lambda = [&last_byte](const uint8_t& value) {
        return value == last_byte;
    };

    if(last_byte <= data.size() && std::all_of(data.cend()-last_byte, data.cend(), validator_lambda)) {
        data.resize(data.size()-last_byte);
    }

    return data;
}

void BasicAes::LoadKey(const std::vector<uint8_t>& key_bytevector, AES_MODE aes_mode) {
    if(key_bytevector.size() != static_cast<uint32_t>(aes_mode)) {
        throw std::invalid_argument("The supplied key isn't the same size as the mode size.");
    } else {
        AES_set_encrypt_key(key_bytevector.data(), static_cast<int32_t>(aes_mode)*8, &aesCbcEncryptionKey_);
        AES_set_decrypt_key(key_bytevector.data(), static_cast<int32_t>(aes_mode)*8, &aesCbcDecryptionKey_);

        std::array<uint8_t, SHA_DIGEST_LENGTH> key_bytearray_digest;

        SHA_CTX ossl_sha1;
        SHA1_Init(&ossl_sha1);
        SHA1_Update(&ossl_sha1, key_bytevector.data(), key_bytevector.size());
        SHA1_Final(key_bytearray_digest.data(), &ossl_sha1);

        std::copy(key_bytearray_digest.begin(), key_bytearray_digest.begin()+AES_BLOCK_SIZE, InitializationVector.begin());
    }
}

void BasicAes::LoadKeySha256(const std::vector<uint8_t>& key_bytevector, AES_MODE aes_mode) {
    std::vector<uint8_t> key_digest(SHA256_DIGEST_LENGTH);
    std::vector<uint8_t> key_digest_stripped(static_cast<uint32_t>(aes_mode));

    SHA256_CTX ossl_sha256;
    SHA256_Init(&ossl_sha256);
    SHA256_Update(&ossl_sha256, key_bytevector.data(), key_bytevector.size());
    SHA256_Final(key_digest.data(), &ossl_sha256);

    std::copy(key_digest.begin(), key_digest.begin()+static_cast<uint32_t>(aes_mode), key_digest_stripped.begin());
    this->LoadKey(key_digest_stripped, aes_mode);
}

AesCredentials BasicAes::DumpCredentials() const {
    AesCredentials returned_credentials(
        this->InitializationVector,
        this->AesCbcEncryptionKey,
        this->AesCbcDecryptionKey
    );

    returned_credentials.Valid = true;

    return returned_credentials;
}

void BasicAes::LoadCredentials(const AesCredentials& credentials) {
    if(credentials.Valid) {
        this->InitializationVector = credentials.InitializationVector;
        this->aesCbcEncryptionKey_ = credentials.EncryptionKey;
        this->aesCbcDecryptionKey_ = credentials.DecryptionKey;
    } else {
        std::cerr << "BasicAes::LoadCredentials -- Tried to load invalid credentials." << std::endl;
    }
}

std::vector<uint8_t> BasicAes::Encrypt(std::vector<uint8_t> unencrypted_data) {
    unencrypted_data = this->ApplyPkcs7(unencrypted_data, AES_BLOCK_SIZE);
    std::vector<uint8_t> encrypted_data(unencrypted_data.size());

    std::array<uint8_t, AES_BLOCK_SIZE> initialization_vector_copy(this->InitializationVector);

    AES_cbc_encrypt(
        unencrypted_data.data(),
        encrypted_data.data(),
        unencrypted_data.size(),
        &this->aesCbcEncryptionKey_,
        initialization_vector_copy.data(),
        AES_ENCRYPT
    );

    return encrypted_data;
}
std::vector<uint8_t> BasicAes::Encrypt(std::string unencrypted_data) {
    return this->Encrypt(std::vector<uint8_t>(unencrypted_data.begin(), unencrypted_data.end()));
}
std::vector<uint8_t> BasicAes::Encrypt(uint8_t* unencrypted_data, std::size_t data_size) {
    return this->Encrypt(std::vector<uint8_t>(unencrypted_data, unencrypted_data+data_size));
}

std::vector<uint8_t> BasicAes::Decrypt(const std::vector<uint8_t>& encrypted_data) {
    std::vector<uint8_t> decrypted_data(encrypted_data.size());
    std::array<uint8_t, AES_BLOCK_SIZE> initialization_vector_copy(this->InitializationVector);

    AES_cbc_encrypt(
        encrypted_data.data(),
        decrypted_data.data(),
        encrypted_data.size(),
        &this->aesCbcDecryptionKey_,
        initialization_vector_copy.data(),
        AES_DECRYPT
    );

    decrypted_data = this->RemovePkcs7(decrypted_data);
    return decrypted_data;
}
std::vector<uint8_t> BasicAes::Decrypt(const std::string& encrypted_data) {
    return this->Decrypt(std::vector<uint8_t>(encrypted_data.begin(), encrypted_data.end()));
}
std::vector<uint8_t> BasicAes::Decrypt(const uint8_t* encrypted_data, std::size_t data_size) {
    return this->Decrypt(std::vector<uint8_t>(encrypted_data, encrypted_data+data_size));
}

BasicAes::BasicAes() : AesCbcEncryptionKey(aesCbcEncryptionKey_), AesCbcDecryptionKey(aesCbcDecryptionKey_) {}