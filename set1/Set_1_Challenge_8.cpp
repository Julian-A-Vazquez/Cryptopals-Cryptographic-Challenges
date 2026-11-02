//Set 1, Challenge 8: Detect AES in ECB mode
//File: https://cryptopals.com/static/challenge-data/8.txt
//In this file are a bunch of hex-encoded ciphertexts.
//One of them has been encrypted with ECB.
//Detect it.
//Remember that the problem with ECB is that it is stateless and deterministic;
//the same 16 byte plaintext block will always produce the same 16 byte ciphertext.

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

bool ECB(const std::string& input) {
for (size_t i = 0; i < input.size(); i += 32) {
    for (size_t j = i + 32; j < input.size(); j += 32) {
        if (input.substr(i, 32) == input.substr(j, 32)){
            return true;
        }
    }
}
return false;
}

std::string Finder(const std::vector<std::string>& fileText){
    std::string answer;
    int linenumber = 0;
    for(std::size_t i = 0; i < fileText.size(); i++){
        if (ECB(fileText[i])){
            answer = fileText[i];
            linenumber = i + 1;
            break;
        }
    }
    std::cout << "The line number is\n" << linenumber;
    return answer;
}
int main(){
    std::ifstream file("Challenge 8 text.txt");
    if (!file) {
        std::cout << "couldn't open file";
        return 1;
    }
    std::vector<std::string> fileText;
    std::string line;
    while (std::getline(file, line)) {
        fileText.push_back(line); 
    }
    std::string answer = Finder(fileText);
    std::cout << "\nAnd the String is:\n" << answer;
}
