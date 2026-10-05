//Cryptopals Challenges Set 1 
//Challenge 2
//Write a function that takes two equal-length buffers and produces their XOR combination.

//If your function works properly, then when you feed it the string:

//1c0111001f010100061a024b53535009181c
//... after hex decoding, and when XOR'd against:

//686974207468652062756c6c277320657965
//... should produce:

//746865206b696420646f6e277420706c6179
#include <iostream>
#include <cstdint>    // uint8_t, uint32_t
#include <stdexcept>  // std::invalid_argument
#include <string>     // std::string
#include <vector>     // std::vector
//First do hex to bytes encoding the hex into its numerical value (0-15)
uint8_t hex_char_value(char c){
   if (c >= '0' && c <= '9')return static_cast<uint8_t>(c - '0');
   if (c >= 'a' && c <= 'f')return static_cast<uint8_t>(c - 'a' + 10);
   if (c >= 'A' && c <= 'F')return static_cast<uint8_t>(c - 'A' + 10);
   throw std::invalid_argument("Not a Hex Value");
}
//Declare function hex_to_bytes that groups hexes by 2 characters
std::vector<uint8_t> hex_to_bytes(const std::string& hex){
    if (hex.size() % 2 != 0){
    throw std::invalid_argument("Not an even Hex string");
}
//Joining the 8 bits from the 2 groups of 4 bit characters that were seperated into upper and lower bits.
    std::vector<uint8_t> bytes;
        for(std::size_t i = 0; i < hex.size(); i += 2){
            uint8_t upper = hex_char_value(hex[i]);
            uint8_t lower = hex_char_value(hex[i+1]);
            bytes.push_back(static_cast<uint8_t>(upper << 4|lower));
        }
        return bytes;
}
//Declare function bytes_to_hex that turns bytes a in a into a hex.
std::string bytes_to_hex(const std::vector<uint8_t> & a){
    const std::string digits = "0123456789abcdef";
//Declare output string where the bits in b are seperated into upper and lower bits and translated into digits. 
std::string out;
        for (std::size_t i = 0; i < a.size(); i++) {
        uint8_t b = a[i];
        out += digits[b >> 4];
        out += digits[b & 0x0F];
    }
    return out;
}
// XOR two equal-length byte buffers
std::vector<uint8_t> xor_bytes(const std::vector<uint8_t>& a, 
                                 const std::vector<uint8_t>& b){
    if (a.size() != b.size()){
        throw std::invalid_argument("Inputs not equal in length");
    }
    std::vector<uint8_t> bytes;
//Iterates the XOR function for every byte of uint8_a and uint8_b
    for(std::size_t i = 0; i <a.size(); i++){
        bytes.push_back(static_cast<uint8_t>(a[i]^b[i]));
    }   
    return bytes;
};
void check(const std::string& hex1, const std::string& hex2, const std::string& expected) {
    std::string result = bytes_to_hex(xor_bytes(hex_to_bytes(hex1), hex_to_bytes(hex2)));
    std::cout << (result == expected ? "PASS  " : "FAIL  ") << result << "\n";
}

int main(){
    std::string input1 = "1c0111001f010100061a024b53535009181c";
    std::string input2 = "686974207468652062756c6c277320657965";
    std::string expected = "746865206b696420646f6e277420706c6179";
    check(input1, input2, expected);
    std::vector<uint8_t> key = hex_to_bytes(input2);
    std::vector<uint8_t> msg = xor_bytes(hex_to_bytes(input1), hex_to_bytes(input2));
    std::cout << std::string(key.begin(), key.end()) << "\n";
    std::cout << std::string(msg.begin(), msg.end()) << "\n";
    return 0;
}

