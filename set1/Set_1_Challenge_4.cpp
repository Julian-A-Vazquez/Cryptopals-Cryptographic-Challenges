//Detect single-character XOR
//One of the 60-character strings in this file has been encrypted by single-character XOR.
//Find it.
//(Your code from #3 should help.)
#include <iostream>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <cctype>
#include <fstream>
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
    std::ifstream file("Challenge 4 text.txt");
    if(!file){
        return 1;
    }
    std::string line;
    double bestscore = -1;
    uint8_t bestkey = 0;
    std::string bestmessage;
    while (std::getline(file,line)){ 
        if(!line.empty() && line.back() == '\r') line.pop_back(); 
        std::vector<uint8_t> convert = hex_to_bytes(line); 
        uint8_t key = scan(convert); 
        std::vector<uint8_t> decrypt = xor_bytes(convert, key); 
        double scoredbytes = score_text(decrypt); 
        if(scoredbytes > bestscore){ 
            bestscore = scoredbytes; 
            bestmessage = std::string(decrypt.begin(), decrypt.end());
            bestkey = key;
        }
    }
        std::cout << "Key:" << static_cast<int>(bestkey) << " ('" << static_cast<char>(bestkey) << "')" << "\n";
        std::cout << bestmessage << "\n"; 
        return 0; 
    } 