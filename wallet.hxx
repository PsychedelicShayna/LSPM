#ifndef WALLETFILE_HH
#define WALLETFILE_HH

/* -------------------------------------------------------- */
/*      Standard Library Includes                           */
/* -------------------------------------------------------- */
#include <experimental/filesystem>
#include <functional>
#include <iterator>
#include <fstream>
#include <vector>
#include <map>
/* -------------------------------------------------------- */


/* -------------------------------------------------------- */
/*      Project Includes                                    */
/* -------------------------------------------------------- */
#include "basic_aes.hxx"
#include "logger.hxx"
#include "json.txx"
/* -------------------------------------------------------- */


/* -------------------------------------------------------- */
/*      Namespace / type aliases.                           */
/* -------------------------------------------------------- */
using Json = nlohmann::json;
namespace fs = std::experimental::filesystem;
/* -------------------------------------------------------- */

class Wallet {
private:
    std::map<std::string, std::map<std::string, std::string>> walletData_;

public:
    /* Ease of use typedef, used to shorten a very long type to make it more readable throughout the code. */
    typedef std::function<std::vector<uint8_t>(std::vector<uint8_t>, std::vector<uint8_t>)> CryptoFunction;

    /* Public, read-only const references to protected members -------------------------------------- */
    const std::map<std::string, std::map<std::string, std::string>>& WalletData;
    /* ---------------------------------------------------------------------------------------------- */

    CryptoFunction EncryptionFunction;
    CryptoFunction DecryptionFunction;

    /* Loads a JSON representation of the wallet data as a byte vector into the wallet file.
     * If the byte vecotr pointer argument is null, no decryption is used, otherwise, the
     * DecryptionFunction is called with the value of the byte vector as the encryption key,
     * and the JSON representation byte vector is used as the source data. */
    void LoadDirect(std::vector<uint8_t>, std::vector<uint8_t>* = nullptr);

    /* Reads all the data from a file specified by the string argument, and then
     * calls LoadDirect to load the JSON representation in that file. */
    void LoadFile(std::string const&, std::vector<uint8_t>* = nullptr);

    /* Returns the JSON representation of the wallet data as a byte vector.
     * If the byte vector pointer argument is null, then no encryption is used,
     * otherwise, the EncryptionFunction is called, with the JSON representation as
     * the data input, and the value of the byte vector pointer as the key. */
    std::vector<uint8_t> DumpDirect(const std::vector<uint8_t>* = nullptr) const;

    /* Calls DumpDirect using the same byte vector pointer argument, except it
     * dumps the output into a file specified by the string parameter. */
    void DumpFile(const std::string&, const std::vector<uint8_t>* = nullptr) const;

    /* Sets the entries ascociated with a certain account. If the account does not exist
     * then the account is created, and then the entries are created. */
    void SetAccount(const std::string&, const std::map<std::string, std::string>&);

    /* Inserts an account entry pair into a certain account. If the account does not exist,
     * then an invlaid_arugment exception is thrown. */
    void SetAccountEntry(const std::string&, std::pair<std::string, std::string>);

    /* Creates a new account in the map with no entries. If the account already exists, does nothing. */
    void CreateAccount(const std::string&);

    /* Check if an account exists in the map, returning a bool. */
    bool AccountExists(const std::string&) const;

    /* Deletes an account from the walletData_ map. If the account does not exist
     * then an invalid_argument exception is thrown. */
    void DeleteAccount(const std::string&);

    /* Returns the list of account names from the wallet file. */
    std::vector<std::string> GetAccountNames() const;

    /* Returns the vector of entry pairs ascociated with a specified account. */
    std::vector<std::pair<std::string, std::string>> GetAccountEntries(const std::string&) const;

    void operator=(const Wallet&);

    /* The encryptionFunction, and decryptionFunction arguments assign to their respective class variables.
     * They are std::function containers, and their code will be used to encrypt or decrypt data in the other
     * class methods, if a key is specified. In the default constructor, they're automatically assigned to
     * AES-256 lambda snippets. */
    Wallet(CryptoFunction encryptionFunction, CryptoFunction decryptionFunction);
    Wallet(const Wallet&);
    Wallet();
};

#endif // WALLETFILE_HH
