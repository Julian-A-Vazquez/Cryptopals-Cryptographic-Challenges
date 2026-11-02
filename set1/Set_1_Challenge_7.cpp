//Set 1, Challenge 7: AES in ECB mode
//File: https://cryptopals.com/static/challenge-data/7.txt
//The Base64-encoded content in this file has been encrypted via AES-128 in ECB mode under the key
//"YELLOW SUBMARINE" (case-sensitive, without the quotes; exactly 16 characters = 16 bytes).
//Decrypt it. You know the key, after all.
//Easiest way: use OpenSSL and give it AES-128-ECB as the cipher.
//Do this with code, not the OpenSSL command-line tool: you'll need ECB working in code a lot later on.
//
//BUILD (the -lcrypto at the end is required):
//   g++ -std=c++17 -Wall -Wextra "Challenge 7 Set 1.cpp" -o challenge7.exe -lcrypto
//   .\challenge7.exe

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <stdexcept>
#include <openssl/evp.h> //Open SSL library

std::vector<uint8_t> base64_to_bytes(const std::string& input) {
    const std::string alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string cleaned;
    for (std::size_t i = 0; i < input.size(); i++) {
        if (input[i] == '=' || alphabet.find(input[i]) != std::string::npos) {
            cleaned += input[i];
        }
    }
    if (cleaned.size() % 4 != 0) {
        throw std::invalid_argument("Not an base64 output string");
    }
    std::vector<uint8_t> out;
    for (std::size_t i = 0; i + 4 <= cleaned.size(); i += 4) {
        uint32_t v0 = (cleaned[i] == '=') ? 0 : (static_cast<uint32_t>(alphabet.find(cleaned[i])));
        uint32_t v1 = (cleaned[i + 1] == '=') ? 0 : (static_cast<uint32_t>(alphabet.find(cleaned[i + 1])));
        uint32_t v2 = (cleaned[i + 2] == '=') ? 0 : (static_cast<uint32_t>(alphabet.find(cleaned[i + 2])));
        uint32_t v3 = (cleaned[i + 3] == '=') ? 0 : (static_cast<uint32_t>(alphabet.find(cleaned[i + 3])));
        uint32_t group = (v0 << 18) | (v1 << 12) | (v2 << 6) | v3;
        out.push_back(static_cast<uint8_t>((group >> 16) & 0xFF));
        if (cleaned[i + 2] != '=') {
            out.push_back(static_cast<uint8_t>((group >> 8) & 0xFF));
        }
        if (cleaned[i + 3] != '=') {
            out.push_back(static_cast<uint8_t>(group & 0xFF));
        }
    }
    return out;
}

std::vector<uint8_t> decrypt(const std::vector<uint8_t>& ciphertext, const std::string& key) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr){
        return {};
    }

    EVP_DecryptInit_ex(ctx, EVP_aes_128_ecb(), nullptr, reinterpret_cast<const unsigned char*>(key.data()), nullptr);
    std::vector<uint8_t> out(ciphertext.size() + 16);

    int updates = 0;
    EVP_DecryptUpdate(ctx, out.data(), &updates, ciphertext.data(), static_cast<int>(ciphertext.size()));

    int updated = 0;
    EVP_DecryptFinal_ex(ctx, out.data() + updates, &updated);

    EVP_CIPHER_CTX_free(ctx);
    out.resize(updates + updated);
    return out;
}

int main() {
    std::ifstream file("Challenge 7 text.txt");
    if (!file) {
        std::cout << "couldn't open file";
        return 1;
    }
    std::string fileText;
    std::string line;
    while (std::getline(file, line)) {
        fileText += line;
    }

    std::string key = "YELLOW SUBMARINE";
    std::vector<uint8_t> decoded = base64_to_bytes(fileText);
    std::vector<uint8_t> bytes = decrypt(decoded, key);
    std::string plaintext(bytes.begin(), bytes.end());
    std::cout << "\n" << plaintext;
}
