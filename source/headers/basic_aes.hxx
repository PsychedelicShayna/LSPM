#ifndef BASIC_AES_HXX
#define BASIC_AES_HXX

// ========================================================
//      Standard Library Includes
// ========================================================
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
// --------------------------------------------------------

// ========================================================
//      OpenSSL Includes
// ========================================================
#include <openssl/aes.h>
#include <openssl/sha.h>
// --------------------------------------------------------

enum struct AES_MODE {
    AES_128 = 16,
    AES_192 = 24,
    AES_256 = 32
};

enum struct KEY_FUNCTION {
    DIRECT = 0,
    HASHED = 1
};

struct AesCredentials {
    std::array<uint8_t, AES_BLOCK_SIZE> InitializationVector;
    AES_KEY EncryptionKey;
    AES_KEY DecryptionKey;
    bool Valid;
    
    void operator=(const AesCredentials&);
    
    AesCredentials();
    AesCredentials(std::array<uint8_t, AES_BLOCK_SIZE>, AES_KEY, AES_KEY);
};

class BasicAes {
private:
    AES_KEY aesCbcEncryptionKey_;
    AES_KEY aesCbcDecryptionKey_;

public:
    // Const read-only references to private members.
    const AES_KEY& AesCbcEncryptionKey;
    const AES_KEY& AesCbcDecryptionKey;

    // The initialization vector used in encryption and decryption.
    std::array<uint8_t, AES_BLOCK_SIZE> InitializationVector;

    // Applies Pkcs7 padding to the input data using the specified multiple.
    static std::vector<uint8_t> ApplyPkcs7(std::vector<uint8_t>, const uint8_t multiple);

    // Removes Pkcs7 padding from the input data.
    static std::vector<uint8_t> RemovePkcs7(std::vector<uint8_t>);

    // Loads the supplied key in the form of a bytevector. If the key size does not match the AES_MODE size, an invalid_argument exception is thrown.
    void LoadKey(const std::vector<uint8_t>&, AES_MODE);

    // Calles LoadKey with AES_MODE# of bytes from the Sha256 hash of the key.
    void LoadKeySha256(const std::vector<uint8_t>&, AES_MODE);

    // Dumps a AesCredentials structure that can be transported.
    AesCredentials DumpCredentials() const;

    // Loads an AesCredentials struct into the current instance.
    void LoadCredentials(const AesCredentials&);

    // The primary encryption functions. A key must have been loaded for this to work.
    std::vector<uint8_t> Encrypt(const std::vector<uint8_t>);
    std::vector<uint8_t> Encrypt(std::string);
    std::vector<uint8_t> Encrypt(uint8_t*, std::size_t);

    // The primary decryption functions. A key must have been loaded for this to work.
    std::vector<uint8_t> Decrypt(const std::vector<uint8_t>&);
    std::vector<uint8_t> Decrypt(const std::string&);
    std::vector<uint8_t> Decrypt(const uint8_t*, std::size_t);

    BasicAes();
    ~BasicAes() = default;
};



#endif // BASIC_AES_HXX