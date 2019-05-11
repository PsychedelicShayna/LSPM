#include <QApplication>
#include <QFile>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "main_window_dialogue.hxx"
#include "password_generator.hxx"
#include "basic_aes.hxx"
#include "wallet.hxx"
#include "logger.hxx"
#include "new_account_dialogue.hxx"


int QtMain(int argc, char** argv) {
    QApplication application(argc, argv);
    MainWindow main_window;

    main_window.show();

    return application.exec();
}

std::vector<uint8_t> ReadAll(std::string const& path) {
    std::vector<uint8_t> return_buffer;

    std::ifstream input_stream(path, std::ios::binary);
    if(!input_stream.good()) {
        std::cout << "Invalid path." << std::endl;
        return return_buffer;
    }

    return_buffer = std::vector<uint8_t>(
        (std::istreambuf_iterator<char>(input_stream)),
        (std::istreambuf_iterator<char>())
    );

    return return_buffer;
}

inline std::ostream& operator<<(std::ostream& stream, std::vector<uint8_t> const& vector) {
    std::stringstream conversion_stream;
    conversion_stream << std::setfill('0');
    conversion_stream << "{ ";

    for(std::size_t i=0; i<vector.size(); ++i) {
        uint8_t const& byte = vector.at(i);
        if(i != 0) conversion_stream << ", ";
        conversion_stream << std::uppercase << std::setw(2) << std::hex << static_cast<uint32_t>(byte);
    }

    conversion_stream << " }";
    stream << conversion_stream.str();

    return stream;
}

void WriteAll(const std::string& path, const std::vector<uint8_t>& data) {
    std::streamsize data_stream_size = static_cast<std::streamsize>(data.size());
    std::ofstream output_stream(path, std::ios::binary);
    if(output_stream.good()) {
        output_stream.write(reinterpret_cast<const char*>(data.data()), data_stream_size);
        output_stream.close();
    } else {
        throw std::invalid_argument("Invalid file path.");
    }
}

template<std::size_t SIZE>
void WriteAll(const std::string& path, const std::array<uint8_t, SIZE>& data) {
    std::streamsize data_stream_size = static_cast<std::streamsize>(SIZE);
    std::ofstream output_stream(path, std::ios::binary);
    if(output_stream.good()) {
        output_stream.write(reinterpret_cast<const char*>(data.data()), data_stream_size);
        output_stream.close();
    } else {
        throw std::invalid_argument("Invalid file path.");
    }
}

std::array<uint8_t, SHA256_DIGEST_LENGTH> Sha256Digest(const std::vector<uint8_t>& data) {
    std::array<uint8_t, SHA256_DIGEST_LENGTH> data_digest;

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.data(), data.size());
    SHA256_Final(data_digest.data(), &sha256);

    return data_digest;
}

int main(int argc, char** argv) {
    return QtMain(argc, argv);
}






















