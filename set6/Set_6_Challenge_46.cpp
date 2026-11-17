//Set 6, Challenge 46: RSA parity oracle
//Page: https://cryptopals.com/sets/6/challenges/46
//A toy problem, but it shows what RSA is really doing. Do it before Challenge 47.
//   1. Generate a 1024-bit RSA key pair.
//   2. Write an oracle function that uses the PRIVATE key and answers only one question:
//      "is the plaintext of this ciphertext even or odd?" (is its last bit 0 or 1). It returns true/false and nothing else.
//      Think of a server that checks the parity of each decrypted message and sends back an error if it's wrong.
//   3. Take this string, un-Base64 it in your code (without looking at it!), and encrypt it to the public key:
//      VGhhdCdzIHdoeSBJIGZvdW5kIHlvdSBkb24ndCBwbGF5IGFyb3VuZCB3aXRoIHRoZSBGdW5reSBDb2xkIE1lZGluYQ==
//   4. Use only the oracle to decrypt it. Why it works:
//      - RSA ciphertexts are just numbers. Multiplying a ciphertext by the encryption of another number
//        multiplies the plaintexts.
//      - Doubling: multiply the ciphertext by (2^e) % n, and the plaintext doubles.
//      - n is odd (it's a product of two odd primes). If the doubled plaintext is EVEN, doubling didn't wrap
//        around n, so the plaintext is less than n/2. If it's ODD, it wrapped, so the plaintext is at least n/2.
//   5. Start with bounds [0, n]. Each oracle call (double again, ask again) cuts the bounds in half:
//      either the upper bound or the lower bound moves to the middle.
//   6. After log2(n) iterations, the bounds have closed in on the message.
//   7. Print the upper bound as a string on every iteration, and you'll see the message decrypt "hollywood style".
//Reuses Challenge 39's RSA (keygen, ModPower, invmod) and a Base64 decoder like Set 1's.
//
//BUILD:
//   g++ -std=c++17 -Wall -Wextra Set_6_Challenge_46.cpp -o challenge46.exe -lgmpxx -lgmp
//   .\challenge46.exe
#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <cassert>
#include <gmpxx.h> // Prime Generation Library
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
mpz_class plaintext_to_numbers(const std::string& message) {
    mpz_class m;
    for(std::size_t i = 0; i < message.length(); i++) {
        m = m * 256 + (static_cast<unsigned char>(message[i])); 
    }
    return m;
}
std::string number_to_plaintext(mpz_class decrypted) {
    std::string out;
    while(decrypted > 0) {
        mpz_class result = decrypted % 256; 
        out += static_cast<unsigned char>((result).get_ui());
        decrypted = decrypted/256;
    }
    for (std::size_t i = 0; i < out.length()/2; i++) { 
        char temp;  
        temp = out[i];
        out[i] = out[out.length() - 1 - i];
        out[out.length() - 1 - i] = temp;                                    
    }
    return out;
}
mpz_class bytes_to_number(const std::vector<uint8_t>& bytes){
    mpz_class m;
    for(std::size_t i = 0; i < bytes.size(); i++) {
        m = m * 256 + bytes[i]; 
    }
    return m;
}
mpz_class testinvmod(mpz_class start, mpz_class modulus){
    mpz_class inverse = 0;
    for(mpz_class i = 0; i < modulus; i++){
    if(start * i % modulus == 1){
        inverse = i;
        break;
    }
    }
    if (inverse == 0) {throw std::invalid_argument("no such value exists");}
    return inverse;
}
mpz_class gcd(mpz_class input1, mpz_class input2) {
    if(input1 == 0||input2 == 0){throw std::invalid_argument("numbers can't be 0");}
    mpz_class remainder = 1;
    while(remainder != 0) {
        remainder = input1 % input2;
        input1 = input2;
        input2 = remainder;
    }
    return input1;
}
mpz_class invmod(mpz_class start, mpz_class modulus) {
    if(start == 0){throw std::invalid_argument("start can't be 0");} 
    mpz_class remainder = 1; 
    mpz_class quotient = 0;
    mpz_class the_gcd = 0;
    mpz_class tempmod = modulus;
    mpz_class coeTHEN = 0;
    mpz_class coeNOW  = 1;
    mpz_class coeTEMP;
    while(remainder != 0){
        quotient = modulus/start;
        coeTEMP = coeTHEN - quotient * coeNOW;
        coeTHEN = coeNOW;
        coeNOW = coeTEMP;
        remainder = modulus % start;
        modulus = start;
        start = remainder;
    }
    the_gcd = modulus;
    if (the_gcd != 1){throw std::invalid_argument("there is no inverse");}
    if(coeTHEN < 0) {coeTHEN = coeTHEN + tempmod;}
    return coeTHEN;
}
mpz_class ModPower(mpz_class base,  mpz_class exponent, mpz_class modulus){
    mpz_class result = 1;
    if(exponent < 0) {throw std::invalid_argument ("Modular Arithmatic typically only uses integers, negative exponents create fractions");}
    base = base % modulus;
    while (exponent > 0) { 
        if (exponent % 2 == 1) {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus; 
        exponent = exponent/2;
    }
    return result;
}
mpz_class Encrypt(mpz_class m, mpz_class e, mpz_class n) {
    mpz_class c = ModPower(m, e, n);
    return c;
}
mpz_class Decrypt(mpz_class c, mpz_class d, mpz_class n) {
    mpz_class m = ModPower(c, d, n);
    return m;
}
mpz_class primegen(gmp_randclass& rng, int bits) {
    mpz_class random = rng.get_z_bits(bits);
    mpz_setbit(random.get_mpz_t(), bits - 1);
    mpz_class ans = 0;
    mpz_nextprime(ans.get_mpz_t(), random.get_mpz_t()); 
    return ans;
}
bool oracle(const mpz_class& ciphertext, const mpz_class& d, const mpz_class& n) {
    mpz_class plaintext;
    plaintext = Decrypt(ciphertext, d, n);
    if(plaintext % 2 == 0) {return true;}
    else{return false;}
}
int main() {
    int bits = 512; 
    if (bits < 2) {throw std::invalid_argument("bit size is too easy to decrypt");}
    std::string secret = "VGhhdCdzIHdoeSBJIGZvdW5kIHlvdSBkb24ndCBwbGF5IGFyb3VuZCB3aXRoIHRoZSBGdW5reSBDb2xkIE1lZGluYQ==";
    mpz_class m = bytes_to_number(base64_to_bytes(secret)); 
    gmp_randclass rng(gmp_randinit_default);
    rng.seed(5418397);
    mpz_class p = primegen(rng, bits);
    mpz_class q = primegen(rng, bits);
    mpz_class n = p * q;
    mpz_class phi_n = (p -1) * (q - 1);
    mpz_class e = 3;
    while(p == q || gcd(e, phi_n) != 1) {
            p = primegen(rng, bits);
            q = primegen(rng, bits);
            phi_n = (p - 1) * (q - 1);
       }
    n = p * q;  
    if( m >= n) {throw std::invalid_argument("message too big");}
    mpz_class d = invmod(e, phi_n);
    mpz_class ciphertext = Encrypt(m, e, n); 
    mpq_class low = 0, high = n;
    mpz_class doubler = ModPower(2, e, n);
    for(std::size_t i = 0; i < mpz_sizeinbase(n.get_mpz_t(), 2); i++) { 
        ciphertext = (ciphertext * doubler) % n;  
        mpq_class mid; 
        mid = (low + high)/2;
        if(oracle(ciphertext, d, n)){ 
            high = mid;
        }
        else{low = mid;}
        std::string lower = number_to_plaintext((mpz_class)low);
        std::string higher = number_to_plaintext((mpz_class)high);
        std::size_t count = 0;
        while(count < lower.length() && count < higher.length() && lower[count] == higher[count]){
            count += 1;
        }
        std::cout << higher.substr(0, count) << "\r"; 
    }
    std::string decrypted = number_to_plaintext((mpz_class)high);
    if(m != (mpz_class)high) {throw std::invalid_argument("decryption failed");} 
    std::cout << "\nattack successful:" << decrypted;
    return 0;
} 