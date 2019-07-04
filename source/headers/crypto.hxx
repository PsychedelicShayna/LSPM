#ifndef CRYPTO_HXX
#define CRYPTO_HXX

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include <iostream>

#include <openssl/sha.h>
#include <openssl/aes.h>

namespace Crypto {
    std::vector<uint8_t> Sha256Digest(const std::vector<uint8_t>& bytes);

    std::vector<uint8_t> ApplyPkcs7(std::vector<uint8_t> bytes, const uint8_t& multiple);
    std::vector<uint8_t> StripPkcs7(std::vector<uint8_t> bytes);

    std::vector<uint8_t> Aes256CbcAutoEncrypt(std::vector<uint8_t> bytes, const std::string& plain_key);
    std::vector<uint8_t> Aes256CbcAutoDecrypt(std::vector<uint8_t> bytes, const std::string& plain_key);
}


#endif // CRYPTO_HXX