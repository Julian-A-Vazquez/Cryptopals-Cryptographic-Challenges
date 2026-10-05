#include <cstdint>    // uint8_t, uint32_t
#include <iostream>   // std::cout
#include <stdexcept>  // std::invalid_argument
#include <string>     // std::string
#include <vector>     // std::vector
// SET 1
// Challenge 1: Convert Hex into base64
// Start:49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d
// End:SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t
// Rule:Always operate on raw bytes, never on encoded strings. Only use hex and base64 for pretty-printing.
//Conversion from characters into 4 bit values
uint8_t hex_char_value(char c) {
    if (c >= '0' && c <= '9') return static_cast<uint8_t>(c - '0');
    if (c >= 'a' && c <= 'f') return static_cast<uint8_t>(c - 'a' + 10);
    if (c >= 'A' && c <= 'F') return static_cast<uint8_t>(c - 'A' + 10);
    throw std::invalid_argument("Not a hex value");
}
//Hex into Bytes broken down 2 at a time
std::vector<uint8_t> hex_to_bytes(const std::string& hex) {
    if (hex.size() % 2 != 0) {
        throw std::invalid_argument("hex string must have an even length");
    }
//Joining the two characters into a byte of lower 4 and upper 4 bits for the hex to bytes conversion
    std::vector<uint8_t> bytes;
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        uint8_t high = hex_char_value(hex[i]);
        uint8_t low =  hex_char_value(hex[i+1]);
        bytes.push_back(static_cast<uint8_t>((high << 4) | low));
    }
    return bytes;
}
//Conversion from bytes to Base 64 values
std::string bytes_to_base64(const std::vector<uint8_t>& bytes) {
    const std::string alphabet =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    //Creating string where output is placed
    std::string out;
    std::size_t i = 0;
    while (i + 3 <= bytes.size()) {
        //A workaround for the lack of a uint24_t, takes 3 bytes and puts them into one number
        uint32_t group = (static_cast<uint32_t>(bytes[i])     << 16) |
                         (static_cast<uint32_t>(bytes[i + 1]) << 8)  |
                         (static_cast<uint32_t>(bytes[i + 2]));
        //Seperating each group of 24 bits into 4 sets of 6 bits that get translated to base 64
        out += alphabet[(group >> 18) & 0x3F];
        out += alphabet[(group >> 12) & 0x3F];
        out += alphabet[(group >>  6) & 0x3F];
        out += alphabet[ group        & 0x3F];

        i += 3;
    }
    //1 leftover byte: 8 real bits -> 2 characters + "=="
    std::size_t leftover = bytes.size() - i;
    if (leftover == 1) {
        uint32_t group = static_cast<uint32_t>(bytes[i]) << 16;
        out += alphabet[(group >> 18) & 0x3F];
        out += alphabet[(group >> 12) & 0x3F];
        out += "==";
    } 
    //Taking the leftover output, adding 0s until its 24 bits/3 bytes and running it through the same algorithm.
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
void check(const std::string & hex, const std::string & expected) {
    std::string result = bytes_to_base64(hex_to_bytes(hex));
    std::cout << (result == expected ? "Pass" : "Fail") << "\n";
}

int main(){
    std::string input = "49276d206b696c6c696e6720796f757220627261696e206c696b65206120706f69736f6e6f7573206d757368726f6f6d";
    std::string expected = "SSdtIGtpbGxpbmcgeW91ciBicmFpbiBsaWtlIGEgcG9pc29ub3VzIG11c2hyb29t";
    check(input, expected);
    return 0;
}
