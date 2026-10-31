//Implement repeating-key XOR
//Here is the opening stanza of an important work of the English language:
//"Burning 'em, if you ain't quick and nimble I go crazy when I hear a cymbal" // NOTE: on the Cryptopals page this is TWO lines. There is a newline between "nimble" and "I go crazy", not a space. The expected hex below contains the byte 0a (newline) in the middle, which proves it.
//Encrypt it, under the key "ICE", using repeating-key XOR.
//In repeating-key XOR, you'll sequentially apply each byte of the key; the first byte of plaintext will be XOR'd against I, the next C, the next E, then I again for the 4th byte, and so on.
//It should come out to:
//0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f
//Encrypt a bunch of stuff using your repeating-key XOR function. Encrypt your mail. Encrypt your password file. Your .sig file. Get a feel for it. I promise, we aren't wasting your time with this.
#include <iostream>
#include <string>
#include <cstdint>
#include <vector>
#include <cctype>
std::vector<uint8_t> xor_bwkey(const std::vector<uint8_t>& a, 
                              const std::string& key){
    std::vector<uint8_t> bytes;
    for(std::size_t i = 0; i <a.size(); i++){ 
        bytes.push_back(static_cast<uint8_t>(a[i] ^ key[(i % key.size())]));
    }
    return bytes;
}
static std::vector<uint8_t> string_to_bytes(const std::string input){
        std::vector<uint8_t> bytes;
        for(std::size_t i = 0; i < input.size(); i++){
        bytes.push_back(static_cast<uint8_t>(input[i]));
        }
        return bytes;
    }
std::string bytes_to_hex(const std::vector<uint8_t>& a){ 
    const std::string digits = "0123456789abcdef";
    std::string out;
        for (std::size_t i = 0; i < a.size(); i++) {
        uint8_t b = a[i];
        out += digits[b >> 4];
        out += digits[b & 0x0F];
    }
    return out;
}
static std::string convert(const std::vector<uint8_t>& a, const std::string& key){
    std::string out = bytes_to_hex(xor_bwkey(a, key)); 
    return out;
}
static void test(const std::string& hex, const std::string& expected){
    if(hex == expected){
        std::cout << "nice \n"; 
    } 
    else{std::cout << "\nYou are all failures, as am I";}
}
int main(){ 
    std::string expected = "0b3637272a2b2e63622c2e69692a23693a2a3c6324202d623d63343c2a26226324272765272a282b2f20430a652e2c652a3124333a653e2b2027630c692b20283165286326302e27282f";
    std::string input1 = "Burning 'em, if you ain't quick and nimble\nI go crazy when I hear a cymbal"; 
    test(convert(string_to_bytes(input1), "ICE"), expected); 
    return 0;
}
