#include "basic_aes.hxx"

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

std::vector<uint8_t> BasicAes::Encrypt(std::vector<uint8_t> data_plain) {
    data_plain = this->ApplyPkcs7(data_plain, AES_BLOCK_SIZE);
    std::vector<uint8_t> data_encrypted(data_plain.size());

    std::array<uint8_t, AES_BLOCK_SIZE> initialization_vector_copy(this->InitializationVector);

    AES_cbc_encrypt(
        data_plain.data(),
        data_encrypted.data(),
        data_plain.size(),
        &this->aesCbcEncryptionKey_,
        initialization_vector_copy.data(),
        AES_ENCRYPT
    );

    return data_encrypted;
}

std::vector<uint8_t> BasicAes::Encrypt(std::string data_plain) {
    return this->Encrypt(std::vector<uint8_t>(data_plain.begin(), data_plain.end()));
}

std::vector<uint8_t> BasicAes::Encrypt(uint8_t* data_plain, std::size_t data_size) {
    return this->Encrypt(std::vector<uint8_t>(data_plain, data_plain+data_size));
}

std::vector<uint8_t> BasicAes::Decrypt(const std::vector<uint8_t>& data_encrypted) {
    std::vector<uint8_t> data_decrypted(data_encrypted.size());
    std::array<uint8_t, AES_BLOCK_SIZE> initialization_vector_copy(this->InitializationVector);

    AES_cbc_encrypt(
        data_encrypted.data(),
        data_decrypted.data(),
        data_encrypted.size(),
        &this->aesCbcDecryptionKey_,
        initialization_vector_copy.data(),
        AES_DECRYPT
    );

    data_decrypted = this->RemovePkcs7(data_decrypted);
    return data_decrypted;
}

std::vector<uint8_t> BasicAes::Decrypt(const std::string& data_encrypted) {
    return this->Decrypt(std::vector<uint8_t>(data_encrypted.begin(), data_encrypted.end()));
}

std::vector<uint8_t> BasicAes::Decrypt(const uint8_t* data_encrypted, std::size_t data_size) {
    return this->Decrypt(std::vector<uint8_t>(data_encrypted, data_encrypted+data_size));
}

BasicAes::BasicAes() : AesCbcEncryptionKey(aesCbcEncryptionKey_), AesCbcDecryptionKey(aesCbcDecryptionKey_) {

}