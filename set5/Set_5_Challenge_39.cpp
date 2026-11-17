//Set 5, Challenge 39: Implement RSA
//Page: https://cryptopals.com/sets/5/challenges/39
//Encrypting and decrypting are the easy part (both are modular exponentiation). Key generation is the real work:
//   1. Pick two primes, p and q. Start with small ones from a table, then switch to big ones.
//      (Using a library to generate primes is allowed; nothing else.)
//   2. n = p * q. All the math happens mod n.
//   3. et = (p - 1) * (q - 1), the totient. Only key generation uses it.
//   4. e = 3.
//   5. d = invmod(e, et): the modular inverse of e mod et. Write invmod yourself, with the extended Euclidean algorithm.
//      Check: invmod(17, 3120) should be 2753.
//   6. Public key = [e, n]. Private key = [d, n].
//   7. Encrypt: c = m^e mod n. Decrypt: m = c^d mod n.
//   8. Test with a small number like 42, then with big primes.
//   9. To encrypt a string: turn it into hex, then read that hex as one big number.
//
//BUILD:
//   g++ -std=c++17 -Wall -Wextra Set_5_Challenge_39.cpp -o challenge39.exe -lgmpxx -lgmp
//   .\challenge39.exe
#include <iostream>
#include <stdexcept>
#include <string>
#include <gmpxx.h> // Prime Generation Library
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

int main() {
    int bits;
    std::cout << "enter number of bits you want to encrypt and decrypt:";
    std::cin >> bits; 
    if (bits < 2) {throw std::invalid_argument("bit size is too easy to decrypt");}
    std::string message;
    std::cout << "\nEncrypt and Decrypt a message and decrypt using RSA:";
    std::getline(std::cin >> std::ws, message); 
    mpz_class m = plaintext_to_numbers(message);
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
    mpz_class decrypted = Decrypt(Encrypt(m, e, n), d, n);
    if(m != decrypted) {throw std::invalid_argument("decryption failed");}
    std::string read = number_to_plaintext(decrypted);
    std::cout << read;
    return 0;
} 