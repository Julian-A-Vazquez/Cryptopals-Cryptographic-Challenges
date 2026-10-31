//Set 1, Challenge 6: Break repeating-key XOR
//File: https://cryptopals.com/static/challenge-data/6.txt
//It's been base64'd after being encrypted with repeating-key XOR. Decrypt it.
//
//Here's how:
//1. Let KEYSIZE be the guessed length of the key; try values from 2 to (say) 40.
//2. Write a function to compute the edit distance/Hamming distance between two strings.
//   The Hamming distance is just the number of differing bits. The distance between:
//       this is a test
//   and
//       wokka wokka!!!
//   is 37 (this is correct, not a typo). Make sure your code agrees before you proceed.
//3. For each KEYSIZE, take the first KEYSIZE worth of bytes, and the second KEYSIZE worth of bytes,
//   and find the edit distance between them. Normalize this result by dividing by KEYSIZE.
//4. The KEYSIZE with the smallest normalized edit distance is probably the key.
//   You could proceed perhaps with the smallest 2-3 KEYSIZE values.
//   Or take 4 KEYSIZE blocks instead of 2 and average the distances.
//5. Now that you probably know the KEYSIZE: break the ciphertext into blocks of KEYSIZE length.
//6. Now transpose the blocks: make a block that is the first byte of every block,
//   and a block that is the second byte of every block, and so on.
//7. Solve each block as if it was single-character XOR. You already have code to do this.
//8. For each block, the single-byte XOR key that produces the best looking histogram is the
//   repeating-key XOR key byte for that block. Put them together and you have the key.

#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <cctype>
#include <fstream>
#include <stdexcept>
#include <cassert>

double freqanalysis(const std::vector<uint8_t>& text) {
    int counts[26]{};
    int total = 0;
    for (std::size_t i = 0; i < text.size(); i++) {
        unsigned char c = static_cast<unsigned char>(std::tolower(text[i]));
        if (c >= 'a' && c <= 'z') {
            counts[c - 'a'] += 1;
            total += 1;
        }
    }
    if (total < 2) {
        return 0.0;
    }
    double sum = 0;
    for (int k = 0; k < 26; k++) {
        sum += static_cast<double>(counts[k]) * (counts[k] - 1);
    }
    return 26.0 * sum / (static_cast<double>(total) * (total - 1));
}

double freqanalysisbyte(const std::vector<uint8_t>& text) {
    int counts[256]{};
    int total = 0;
    for (std::size_t i = 0; i < text.size(); i++) {
        unsigned char c = text[i];
        counts[c] += 1;
        total += 1;
    }
    if (total < 2) {
        return 0.0;
    }
    double sum = 0;
    for (int k = 0; k < 256; k++) {
        sum += static_cast<double>(counts[k]) * (counts[k] - 1);
    }
    return 256.0 * sum / (static_cast<double>(total) * (total - 1));
}

std::vector<std::vector<uint8_t>> split_column(const std::vector<uint8_t>& text, int columnum) {
    std::vector<std::vector<uint8_t>> out(columnum);
    for (std::size_t i = 0; i < text.size(); i++) {
        out[i % columnum].push_back(text[i]);
    }
    return out;
}

static int Findkeylen(const std::vector<uint8_t>& text) {
    double Closestscore = 0;
    int keylength = 0;
    for (std::size_t i = 2; i <= 40; i++) {
        double score = 0;
        std::vector<std::vector<uint8_t>> columns = split_column(text, i);
        for (std::size_t j = 0; j < columns.size(); j++) {
            score += (freqanalysisbyte(columns[j]));
        }
        score = score / columns.size();
        if (score > Closestscore) {
            Closestscore = score;
            keylength = i;
        }
    }
    return keylength;
}

std::string bytes_to_base64(const std::vector<uint8_t>& bytes) {
    const std::string alphabet =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    std::size_t i = 0;
    while (i + 3 <= bytes.size()) {
        uint32_t group = (static_cast<uint32_t>(bytes[i])     << 16) |
                         (static_cast<uint32_t>(bytes[i + 1]) << 8)  |
                         (static_cast<uint32_t>(bytes[i + 2]));
        out += alphabet[(group >> 18) & 0x3F];
        out += alphabet[(group >> 12) & 0x3F];
        out += alphabet[(group >>  6) & 0x3F];
        out += alphabet[ group        & 0x3F];

        i += 3;
    }
    std::size_t leftover = bytes.size() - i;
    if (leftover == 1) {
        uint32_t group = static_cast<uint32_t>(bytes[i]) << 16;
        out += alphabet[(group >> 18) & 0x3F];
        out += alphabet[(group >> 12) & 0x3F];
        out += "==";
    }
    else if (leftover == 2) {
        uint32_t group = (static_cast<uint32_t>(bytes[i])     << 16) |
                         (static_cast<uint32_t>(bytes[i + 1]) << 8);
        out += alphabet[(group >> 18) & 0x3F];
        out += alphabet[(group >> 12) & 0x3F];
        out += alphabet[(group >>  6) & 0x3F];
        out += "=";
    }
    return out;
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

std::vector<uint8_t> xor_bwkey(const std::vector<uint8_t>& a, const std::string& key) {
    std::vector<uint8_t> bytes;
    for (std::size_t i = 0; i < a.size(); i++) {
        bytes.push_back(static_cast<uint8_t>(a[i] ^ key[(i % key.size())]));
    }
    return bytes;
}

std::vector<uint8_t> xor_bytes(const std::vector<uint8_t>& a,
                               uint8_t key) {
    std::vector<uint8_t> bytes;
    for (std::size_t i = 0; i < a.size(); i++) {
        bytes.push_back(static_cast<uint8_t>(a[i] ^ key));
    }
    return bytes;
}

double score_text(const std::vector<uint8_t>& text) {
    double score = 0;
    const double freq[26] {8.2, 1.5, 2.8, 4.3, 12.7, 2.2, 2.0, 6.1, 7.0,
        0.15, 0.8, 4.0, 2.4, 6.7, 7.5, 1.9, 0.10, 6.0,
        6.3, 9.1, 2.8, 1.0, 2.4, 0.15, 2.0, 0.07};

    for (std::size_t i = 0; i < text.size(); i++) {
        unsigned char c = tolower(text[i]);
        if (c >= 'a' && c <= 'z') {
            score += freq[c - 'a'];
        }
        else if (c == ' ') {
            score += 13.0;
        }
    }
    return score;
}

uint8_t scan(const std::vector<uint8_t>& decoded) {
    double bestscore = -1;
    uint8_t bestkey = 0;
    for (std::size_t i = 0; i < 256; i++) {
        std::vector<uint8_t> output =
            xor_bytes(decoded, static_cast<uint8_t>(i));
        double score = score_text(output);
        if (score > bestscore) {
            bestscore = score;
            bestkey = uint8_t(i);
        }
    }
    return bestkey;
}

int main() {
    std::ifstream file("Challenge 6 Set 1 text.txt");
    if (!file) {
        std::cout << "couldn't open file";
        return 1;
    }
    std::string expected = "I'm killing your brain like a poisonous mushroom";
    std::string fileText;
    std::string line;
    while (std::getline(file, line)) {
        fileText += line;
    }

    std::vector<uint8_t> decoded = base64_to_bytes(fileText);
    int keylength = Findkeylen(decoded);
    std::cout << "Keylength is:\n" << keylength;

    std::string key;
    std::vector<std::vector<uint8_t>> result = split_column(decoded, keylength);
    for (std::size_t i = 0; i < result.size(); i++) {
        key += scan(result[i]);
    }

    std::vector<uint8_t> bytes = xor_bwkey(decoded, key);
    std::string plaintext(bytes.begin(), bytes.end());
    std::cout << "\nThe key is:\n" << key;
    std::cout << "\n" << plaintext;
    return 0;
}
