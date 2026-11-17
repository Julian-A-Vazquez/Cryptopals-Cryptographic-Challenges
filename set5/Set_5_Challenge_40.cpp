//Set 5, Challenge 40: Implement an E=3 RSA Broadcast attack
//Page: https://cryptopals.com/sets/5/challenges/40
//Setup: someone uses textbook RSA (no padding) with e = 3 and encrypts the SAME message 3 times, under 3 different public keys.
//With the 3 ciphertexts and the 3 public keys, you can get the message back WITHOUT any private key:
//   1. Collect the 3 ciphertexts c_0, c_1, c_2 and their moduli n_0, n_1, n_2.
//   2. Combine them with the Chinese Remainder Theorem (CRT) into one number, result, mod N_012 = n_0 * n_1 * n_2:
//        m_s_k = the product of the two moduli other than n_k   (m_s_0 = n_1 * n_2, and so on)
//        result = ( c_0 * m_s_0 * invmod(m_s_0, n_0)
//                 + c_1 * m_s_1 * invmod(m_s_1, n_1)
//                 + c_2 * m_s_2 * invmod(m_s_2, n_2) ) mod N_012
//   3. Take the CUBE ROOT of result. That's the message.
//      Why no mod is needed: m^3 is smaller than N_012, so result IS m^3 exactly, as a plain integer.
//Reuses Challenge 39's RSA and invmod.
//
//BUILD:
//   g++ -std=c++17 -Wall -Wextra Set_5_Challenge_40.cpp -o challenge40.exe -lgmpxx -lgmp
//   .\challenge40.exe
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <cassert>
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
mpz_class Power(mpz_class base, mpz_class exponent){
    if(exponent < 0) {throw std::invalid_argument ("Modular Arithmatic typically only uses integers, negative exponents create fractions");}
    mpz_class result = 1;
    while (exponent > 0) { 
        if (exponent % 2 == 1) {
            result = (result * base);
        }
        base = (base * base); 
        exponent = exponent/2;
    }
    return result;
}
mpq_class PowerFrac(mpz_class base, mpz_class exponent) {
    if(exponent > 0) {throw std::invalid_argument ("use Power() for positive exponents");}
    if(base == 0) {throw std::invalid_argument ("cannot divide by 0");}    
    mpq_class result = 1; 
    result = mpq_class(1)/Power(base, -exponent); 
    return result;
}   
mpz_class nthroot(mpz_class number, mpz_class root) {
    if (root == 0) {throw std::invalid_argument("Zeroth root doesn't exist");}
    if (root < 0) {throw std::invalid_argument("Use negnthroot() for negative roots");}
    if (number < 0 && (mpz_class)root % 2 == 0) {throw std::invalid_argument("even roots for negative numbers don't exist without imaginary numbers");}
    if(number < 0 && root % 2 == 1){ 
    return -nthroot(-number, root);
    }
    mpz_class low = 0;
    mpz_class high = number;
    mpz_class mid = (low + high + 1)/2;
    while(low < high){
        if(Power(mid, root) <= number) {low = mid;}
        else{high = mid - 1;} 
        mid = (low + high + 1)/2;
    }
    return low;
}
mpq_class negnthroot(mpz_class number, mpz_class root) {
    if(root > 0) {throw std::invalid_argument ("use nthroot() for positive exponents");}     
    if (root == 0) {throw std::invalid_argument("Zeroth root doesn't exist");} 
    if (number == 0) {throw std::invalid_argument("root of 0 is always 0");} 
    mpq_class result = 1;
    result = mpq_class(1)/nthroot(number, -root); 
    return result;
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

mpz_class ModPower(mpz_class base, mpz_class exponent, mpz_class modulus){
    mpz_class result = 1;
    if(exponent < 0) {throw std::invalid_argument ("Modular Arithmatic typically only uses integers, negative exponents create fractions");}
    for(mpz_class i = 0; i < exponent; i++) {
        result = (result * base) % modulus;
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
mpz_class pickup(const std::vector<mpz_class>& messages, const std::vector<mpz_class>& messagemod, const mpz_class e) { 
    if (messages.size() != messagemod.size()) {throw std::invalid_argument ("the message and key length don't match");}
    if (messages.size() < 3 || messagemod.size() < 3) {throw std::invalid_argument("this algorithm cannot get the key with this little length");}
    std::vector<mpz_class> m_s;
    mpz_class  result = 0;
    mpz_class  modulus = 1;
    for(std::size_t i = 0; i < messagemod.size(); i++) {
        modulus = modulus * messagemod[i];
    }
    for (std::size_t i = 0; i < messages.size(); i++) {
         m_s.push_back(modulus/messagemod[i]);
    }

    for(std::size_t  i = 0; i < messages.size(); i++) {
        result += messages[i] * m_s[i] * invmod(m_s[i], messagemod[i]);
    }
    result = result % modulus;
    return nthroot(result, e); 
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
mpz_class keygen(gmp_randclass& rng, int bits) {
    mpz_class p = primegen(rng, bits);
    mpz_class q = primegen(rng, bits);
    while(p == q || gcd(3, (p -1) * (q -1)) != 1) {
            p = primegen(rng, bits);
            q = primegen(rng, bits);
    }
    mpz_class n = p * q;
    return n;
}
int main() {
    int keycount;
    int bits;
    std::string message;
    std::cout << "give keycount (must be 3 or greater):"; 
    std::cin >> keycount;
    std::cout << "\ngive bitcount:";
    std::cin >> bits;
    std::cout << "\ngive message:";
    std::getline(std::cin >> std::ws, message); 
    mpz_class m = plaintext_to_numbers(message);
    if(keycount < 3) {throw std::invalid_argument ("keycount must be 3 or greater");} 
    if(bits < 64) {throw std::invalid_argument("primes this small are too easy");}
    gmp_randclass rng(gmp_randinit_default);
    rng.seed(5418397);
    std::vector<mpz_class> moduli;
    std::vector<mpz_class> encrypted;
    for(int i = 0; i < keycount; i++) { 
        moduli.push_back(keygen(rng, bits));
    }                                
    mpz_class e = 3;
    for(int i = 0; i < keycount; i++){ if(m >= moduli[i]){throw std::invalid_argument("message too big");}}
    for(int i = 0; i < keycount; i++) {
       encrypted.push_back(Encrypt(m, e, moduli[i]));
    }
    mpz_class recovered = pickup(encrypted, moduli, e);
    std::string read = number_to_plaintext(recovered);
    if(read != message){throw std::invalid_argument("it failed");}
    else{std::cout << "well done, here it is:" << read;} 
    return 0;
}