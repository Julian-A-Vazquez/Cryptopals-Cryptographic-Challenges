//Cryptopals Challenges Set 1 
//Challenge 3
//The hex encoded string:
//1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736
//has been XOR'd against a single character. 
//Find the key, decrypt the message.
//You can do this by hand. But don't: write code to do it for you.
//How? Devise some method for "scoring" a piece of English plaintext. 
//Character frequency is a good metric. 
//Evaluate each output and choose the one with the best score.
#include <iostream>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <cctype>
uint8_t hex_char_value(char c){
    if(c >= '0' && c <= '9')return static_cast<uint8_t>(c - '0');
    if(c >= 'a' && c <= 'f')return static_cast<uint8_t>(c - 'a' + 10);
    if(c >= 'A' && c <= 'F')return static_cast<uint8_t>(c - 'A' + 10);
    throw std::invalid_argument("Not a real Hex String");
}
std::vector<uint8_t>hex_to_bytes(const std::string& hex) {
    if (hex.size() % 2 != 0) {
        throw std::invalid_argument("hex string must have an even length");
    }
    std::vector<uint8_t> bytes;
    for (std::size_t i = 0; i < hex.size(); i += 2) {
        uint8_t high = hex_char_value(hex[i]);
        uint8_t low =  hex_char_value(hex[i+1]);
        bytes.push_back(static_cast<uint8_t>((high << 4) | low));
    }
    return bytes;
}
std::vector<uint8_t> xor_bytes(const std::vector<uint8_t>& a, 
                              uint8_t key){
    std::vector<uint8_t> bytes;
    for(std::size_t i = 0; i <a.size(); i++){
        bytes.push_back(static_cast<uint8_t>(a[i] ^ key));
        }
    return bytes;
}
double score_text(const std::vector<uint8_t>& text){
    double score = 0;
    const double freq[26] {8.2, 1.5, 2.8, 4.3, 12.7, 2.2, 2.0, 6.1, 7.0,
        0.15, 0.8, 4.0, 2.4, 6.7, 7.5, 1.9, 0.10, 6.0,
        6.3, 9.1, 2.8, 1.0, 2.4, 0.15, 2.0, 0.07};

    for(std::size_t i = 0; i <text.size(); i++){
        unsigned char c = tolower(text[i]);
        if(c >= 'a' && c <= 'z'){
        score += freq[c - 'a'];
        }
        else if(c == ' '){
            score += 13.0;
        }
    }
    return score;
}
uint8_t scan(const std::vector<uint8_t>& ciphertext){
    int i;
    double bestscore = -1;
    uint8_t bestkey = 0;
    
    for(i = 0; i < 256; i++){
        std::vector<uint8_t> output = 
            xor_bytes(ciphertext, static_cast<uint8_t>(i));
        double score = score_text(output); 
        if(score > bestscore){
            bestscore = score;
            bestkey = uint8_t (i);
        }
    }
    return bestkey;
}
int main(){
    std::string input1 = "1b37373331363f78151b7f2b783431333d78397828372d363c78373e783a393b3736"; 
    std::vector<uint8_t> bytes = hex_to_bytes(input1);
    uint8_t key = scan(bytes);
    std::vector<uint8_t> decrypted = xor_bytes(bytes, key);
    std::string message(decrypted.begin(), decrypted.end());

    std::cout << "Key: " << static_cast<int>(key)
              << " ('" << static_cast<char>(key) << "')\n";
    std::cout << "Message: " << message << "\n";
    return 0;
}