#ifndef BASIC_AES_HXX
#define BASIC_AES_HXX

#include <openssl/aes.h>
#include <openssl/sha.h>
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <array>

class BasicAes {
private:
    AES_KEY aesCbcEncryptionKey_;
    AES_KEY aesCbcDecryptionKey_;

public:
    enum AES_MODE {
        AES_128 = 16,
        AES_192 = 24,
        AES_256 = 32
    };

    // Const read-only references to private members.
    const AES_KEY& AesCbcEncryptionKey;
    const AES_KEY& AesCbcDecryptionKey;

    // The initialization vector used in encryption and decryption.
    std::array<uint8_t, AES_BLOCK_SIZE> InitializationVector;

    // Applies Pkcs7 padding to the input data using the specified multiple.
    static std::vector<uint8_t> ApplyPkcs7(std::vector<uint8_t>, const uint8_t multiple);

    // Removes Pkcs7 padding from the input data.
    static std::vector<uint8_t> RemovePkcs7(std::vector<uint8_t>);

    /* The main function for loading AES keys, simply loads the key, and loads the InitializationVector
     * with AES_BLOCK_SIZE# of bytes from the key std::array. */
    template<AES_MODE MODE>
    void LoadKeyDirect(const std::array<uint8_t, static_cast<std::size_t>(MODE)>& key_bytearray) {
        AES_set_encrypt_key(key_bytearray.data(), static_cast<std::size_t>(MODE)*8, &this->aesCbcEncryptionKey_);
        AES_set_decrypt_key(key_bytearray.data(), static_cast<std::size_t>(MODE)*8, &this->aesCbcDecryptionKey_);
        memcpy(this->InitializationVector.data(), key_bytearray.data(), AES_BLOCK_SIZE);
    }

    // Feeds MODE# of bytes from the SHA-256 digest of the supplied key std::array into LoadKeyDirect.
    template<AES_MODE MODE>
    void LoadKey(const std::array<uint8_t, static_cast<std::size_t>(MODE)>& key_bytearray) {
        std::array<uint8_t, SHA256_DIGEST_LENGTH> key_bytearray_digest;

        SHA256_CTX sha256_object;
        SHA256_Init(&sha256_object);
        SHA256_Update(&sha256_object, key_bytearray.data(), key_bytearray.size());
        SHA256_Final(key_bytearray_digest.data(), &sha256_object);

        std::array<uint8_t, static_cast<std::size_t>(MODE)> key_bytearray_digest_chunk;
        memcpy(key_bytearray_digest_chunk.data(), key_bytearray_digest.data(), static_cast<std::size_t>(MODE));
        this->LoadKeyDirect<MODE>(key_bytearray_digest_chunk);
    }

    // Feeds MODE# of bytes from the SHA-256 digest of the supplied key std::vector into LoadKeyDirect.
    template<AES_MODE MODE>
    void LoadKey(const std::vector<uint8_t>& key_bytevector) {
        std::array<uint8_t, SHA256_DIGEST_LENGTH> key_bytevector_digest;

        SHA256_CTX sha256_object;
        SHA256_Init(&sha256_object);
        SHA256_Update(&sha256_object, key_bytevector.data(), key_bytevector.size());
        SHA256_Final(key_bytevector_digest.data(), &sha256_object);

        std::array<uint8_t, static_cast<std::size_t>(MODE)> key_bytevector_digest_chunk;
        memcpy(key_bytevector_digest_chunk.data(), key_bytevector_digest.data(), static_cast<std::size_t>(MODE));
        this->LoadKeyDirect<MODE>(key_bytevector_digest_chunk);
    }

    // Converts a std::string into a byte vector and calls the std::vector overload of LoadKey.
    template<AES_MODE MODE>
    void LoadKey(const std::string& key_string) {
        this->LoadKey<MODE>(std::vector<uint8_t>(key_string.begin(), key_string.end()));
    }

    // Converts a c-bytearray into std::array and calls the std::array overload of LoadKey.
    template<AES_MODE MODE>
    void LoadKey(const uint8_t key_bytecarray[MODE]) {
        std::array<uint8_t, static_cast<std::size_t>(MODE)> key_bytearray;
        memcpy(key_bytearray.data(), key_bytecarray, static_cast<std::size_t>(MODE));
        this->LoadKey<MODE>(key_bytearray);
    }

    // Converts the c-signedbytearray into an std::array and calls the std::array overload of LoadKey.
    template<AES_MODE MODE>
    void LoadKey(const int8_t key_cstring[MODE]) {
        std::array<uint8_t, static_cast<std::size_t>(MODE)> key_bytearray;
        memcpy(key_bytearray.data(), key_cstring, static_cast<std::size_t>(MODE));
        this->LoadKey<MODE>(key_bytearray);
    }

    // The primary encryption function. A key must have been loaded for this to work.
    std::vector<uint8_t> Encrypt(const std::vector<uint8_t>);
    std::vector<uint8_t> Encrypt(std::string);
    std::vector<uint8_t> Encrypt(uint8_t*, std::size_t);

    // decryption
    // The primary decryption function. A key must have been loaded for this to work.
    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>&);
    std::vector<uint8_t> Decrypt(const std::string&);
    std::vector<uint8_t> Decrypt(const uint8_t*, std::size_t);

    BasicAes();
    ~BasicAes() = default;
};

#endif // BASIC_AES_HXX
