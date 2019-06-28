#ifndef PWDGENERATOR_HH
#define PWDGENERATOR_HH

#include <algorithm>
#include <random>
#include <vector>
#include <string>

class PasswordGenerator {
protected:
    std::random_device random_device_;
    std::mt19937 random_generator_;
    std::uniform_int_distribution<uint32_t> random_distributer_;
    std::string seed_;

public:
    /* Public, read-only const references to protected members */
    std::string const& Seed = seed_;
    /* ------------------------------------------------------- */

    enum CharSet {
        ALOWER      = 0,
        AUPPER      = 1,
        DECIMAL     = 2,
        SPECIAL     = 3,
        SEPCIAL2    = 4
    };

    void Configure(std::vector<std::pair<CharSet, uint32_t>> const&);
    void Configure(std::string const&);

    std::string Generate(std::size_t);
    PasswordGenerator();
};

#endif // PWDGENERATOR_HH
