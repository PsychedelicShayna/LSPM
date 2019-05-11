#ifndef RAINBOWTABLE_HH
#define RAINBOWTABLE_HH

#include <QCryptographicHash>
#include <iostream>
#include <iomanip>
#include <cstdint>
#include <sstream>
#include <string>
#include <vector>
#include <map>


// Use 30 for separator, aka 0x1E (record separator)

class RainbowTable {
protected:
    std::map<uint8_t, std::string> table_;
    std::map<std::string, uint8_t> revTable_;
    QCryptographicHash::Algorithm algorithm_;
    std::size_t digestSize_;

public:
    /* Public read-only const references of protected members */
    std::map<uint8_t, std::string> const& Table = table_;
    std::map<std::string, uint8_t> const& RevTable = revTable_;
    QCryptographicHash::Algorithm const& Algorithm = algorithm_;
    std::size_t const& DigestSize = digestSize_;
    /* ------------------------------------------------------ */

    // Loads a specified key, generating the corresponding table.
    void LoadKey(std::string const&);

    // Vector based encryption/decryption.
    std::vector<std::string> Encrypt(std::vector<uint8_t> const&) const;
    std::vector<uint8_t> Decrypt(std::vector<std::string> const&) const;

    // Pointer based encryption/decryption.
    uint8_t** Encrypt(uint8_t* const&, std::size_t const&) const;
    uint8_t* Decrypt(uint8_t** const&, std::size_t const&) const;

    // Joins a hash array into a 0x1E separated string.
    std::string Wind(std::vector<std::string> const&) const;
    uint8_t* Wind(uint8_t** const&, std::size_t const&) const;

    // Unwinds a joined hash array back into an array.
    std::vector<std::string> Unwind(std::istream&) const;
    std::vector<std::string> Unwind(std::string const&) const;
    uint8_t** Unwind(uint8_t* const&, std::size_t const&) const;

    // Compresses a hash array into a more compact format.
    uint8_t* Compress(std::string const&) const;
    uint8_t* Compress(uint8_t** const&, std::size_t const&) const;

    RainbowTable(QCryptographicHash::Algorithm);
    ~RainbowTable();
};

#endif // RAINBOWTABLE_HH
