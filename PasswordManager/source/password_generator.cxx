#include "headers/password_generator.hxx"

void PasswordGenerator::Configure(std::vector<std::pair<CharSet, uint32_t>> const& selections) {
    static const char* character_sets[5] = {
        "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "1234567890",
        "!@#$%^&*?",
        "+-/[]{}()';:~_+=><,.`'\"",
    };

    this->seed_.clear();

    for(std::pair<CharSet, uint32_t> const& selection : selections) {
        for(std::size_t i=0; i<selection.second; ++i) {
            this->seed_.append(character_sets[selection.first]);
        }
    }

    this->random_distributer_ = std::uniform_int_distribution<uint32_t>(0, static_cast<uint32_t>(this->seed_.size()-1));
    std::random_shuffle(this->seed_.begin(), this->seed_.end());
}

void PasswordGenerator::Configure(std::string const& seed) {
    this->seed_ = std::string(seed);
    this->random_distributer_ = std::uniform_int_distribution<uint32_t>(0, static_cast<uint32_t>(this->seed_.size()-1));
}

std::string PasswordGenerator::Generate(std::size_t length) {
    std::vector<char> password(length);

    for(std::size_t i=0; i<length; ++i) {
        uint32_t index = this->random_distributer_(this->random_generator_);
        password.emplace_back(this->seed_.at(index));
    }

    return std::string(password.begin(), password.end());
}

PasswordGenerator::PasswordGenerator() {
    this->random_generator_ = std::mt19937(this->random_device_());
}
