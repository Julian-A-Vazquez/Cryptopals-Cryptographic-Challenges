//Set 1, Challenge 7: AES in ECB mode
//File: https://cryptopals.com/static/challenge-data/7.txt
//The Base64-encoded content in this file has been encrypted via AES-128 in ECB mode under the key
//"YELLOW SUBMARINE" (case-sensitive, without the quotes; exactly 16 characters = 16 bytes).
//Decrypt it. You know the key, after all.
//Easiest way: use OpenSSL and give it AES-128-ECB as the cipher.
//Do this with code, not the OpenSSL command-line tool: you'll need ECB working in code a lot later on.
//
//BUILD from set1/ (uses my own AES from the aes/ submodule instead of OpenSSL; compile the C file with gcc, then link with g++):
//   gcc -std=c11 -Wall -Wextra -c ../aes/src/aes.c -o aes.o
//   g++ -std=c++17 -Wall -Wextra Set_1_Challenge_7.cpp aes.o -o challenge7.exe
//   .\challenge7.exe

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <stdexcept>
extern "C" {              
#include "../aes/src/aes.h"    
}
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
    if(key.length() != 16) {
        throw std::invalid_argument("invalid key length");
    } 
    if(ciphertext.size() % 16 != 0|| ciphertext.size() == 0) {
        throw std::invalid_argument("empty, or invalid ciphertext");
    }
    uint8_t w[240];  
    key_expansion(reinterpret_cast<const uint8_t*>(key.data()), w, 4);
    std::vector<uint8_t> out(ciphertext.size());
    for(size_t i = 0; i < ciphertext.size(); i += 16) {
        aes_decrypt_block(ciphertext.data() + i, out.data() + i, w, 10);
    }
    uint8_t last = out[out.size() - 1];
    if(last > out.size() || last < 1 || last > 16){ 
        throw std::invalid_argument("invalid value");
    }
    out.resize(out.size() - last);
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
