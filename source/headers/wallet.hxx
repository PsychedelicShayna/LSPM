#ifndef WALLETFILE_HH
#define WALLETFILE_HH

/* -------------------------------------------------------- */
/*      Standard Library Includes                           */
/* -------------------------------------------------------- */
#include <functional>
#include <iterator>
#include <fstream>
#include <vector>
#include <map>
/* -------------------------------------------------------- */


/* -------------------------------------------------------- */
/*      Project Includes                                    */
/* -------------------------------------------------------- */
#include "json.txx"
/* -------------------------------------------------------- */

/* -------------------------------------------------------- */
/*      Namespace / type aliases.                           */
/* -------------------------------------------------------- */
using Json = nlohmann::json;
/* -------------------------------------------------------- */

class Wallet {
private:
    std::map<std::string, std::map<std::string, std::string>> walletData_;

public:
    /* Public, read-only const references to protected members -------------------------------------- */
    const std::map<std::string, std::map<std::string, std::string>>& WalletData;
    /* ---------------------------------------------------------------------------------------------- */

    /* Loads a JSON representation of the wallet data as a byte vector into the wallet file. */
    void LoadDirect(std::vector<uint8_t>);

    /* Reads all the data from a file specified by the string argument, and then
     * calls LoadDirect to load the JSON representation in that file. */
    void LoadFile(std::string const&);

    /* Returns the JSON representation of the wallet data as a byte vector. */
    std::vector<uint8_t> DumpDirect() const;

    /* Calls DumpDirect using the same byte vector pointer argument, except it
     * dumps the output into a file specified by the string parameter. */
    void DumpFile(const std::string&) const;

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
    Wallet(const Wallet&);
    Wallet();
};

#endif // WALLETFILE_HH
