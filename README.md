# Cryptopals

Solutions to the [Cryptopals challenges](https://cryptopals.com), in C++.
Sets 2–7 on the above link. Set 8 (the abstract algebra set) is not published on the
main site but instead here: (https://gist.github.com/arkadiyt/5b33bed653ce1dc26e1df9c249d8919e).

Cryptographic primitives are implemented from scratch rather than pulled from
a library. AES comes from my own [FIPS 197 implementation](https://github.com/Julian-A-Vazquez/aes-128-fips197),
included here as the `aes/` git submodule; challenge 7 first used OpenSSL and now uses it.

Clone with the submodule:

```
git clone --recurse-submodules https://github.com/Julian-A-Vazquez/cryptopals-cryptographic-challenges
```

(or run `git submodule update --init` in an existing clone).

## Layout

One directory per set. Each solution is self-contained and carries its own `main`
with the challenge's published input, and prints either `PASS` or the recovered key and plaintext.

```
aes/                          (submodule: my AES from FIPS 197, src/aes.c and src/aes.h)
set1/Set_1_Challenge_1.cpp
set1/Set_1_Challenge_2.cpp
set1/Set_1_Challenge_3.cpp
set1/Set_1_Challenge_4.cpp
set1/Challenge 4 text.txt     (input for challenge 4)
set1/Set_1_Challenge_5.cpp
set1/Set_1_Challenge_6.cpp
set1/Challenge 6 text.txt     (input for challenge 6)
set1/Set_1_Challenge_7.cpp
set1/Challenge 7 text.txt     (input for challenge 7)
set1/Set_1_Challenge_8.cpp
set1/Challenge 8 text.txt     (input for challenge 8)
set5/Set_5_Challenge_39.cpp
set5/Set_5_Challenge_40.cpp
set6/Set_6_Challenge_46.cpp
...
```

## Build

Each file compiles on its own:

```
g++ -std=c++17 -O2 -Wall -Wextra -o challenge set1/Set_1_Challenge_1.cpp
./challenge
```

Challenges 4, 6, 7 and 8 open their input files (`Challenge 4 text.txt`, `Challenge 6 text.txt`,
`Challenge 7 text.txt`, `Challenge 8 text.txt`) from the current directory, so run them from `set1/`.

Challenge 7 uses the AES in the submodule. Compile the C file, then link it in, from `set1/`:

```
gcc -std=c11 -O2 -Wall -Wextra -c ../aes/src/aes.c -o aes.o
g++ -std=c++17 -O2 -Wall -Wextra Set_1_Challenge_7.cpp aes.o -o challenge
./challenge
```

Sets 5 and 6 (RSA) use [GMP](https://gmplib.org) for big-integer arithmetic and to find primes
(`mpz_nextprime`). Everything else, including the modular inverse, modular exponentiation
and the integer cube root, is written by hand. Install GMP with its C++ bindings (`libgmp-dev`
on Debian/Ubuntu, or `mingw-w64-x86_64-gmp` in MSYS2), then link it:

```
g++ -std=c++17 -O2 -Wall -Wextra set5/Set_5_Challenge_39.cpp -o challenge -lgmpxx -lgmp
./challenge
```

## Progress

**Set 1 — Basics**
- [x] 1. Convert hex to base64
- [x] 2. Fixed XOR
- [x] 3. Single-byte XOR cipher
- [x] 4. Detect single-character XOR
- [x] 5. Implement repeating-key XOR
- [x] 6. Break repeating-key XOR
- [x] 7. AES in ECB mode
- [x] 8. Detect AES in ECB mode

**Set 5 — Diffie-Hellman and friends**
- [ ] 33. Implement Diffie-Hellman
- [ ] 34. Implement a MITM key-fixing attack on Diffie-Hellman with parameter injection
- [ ] 35. Implement DH with negotiated groups, and break with malicious "g" parameters
- [ ] 36. Implement Secure Remote Password (SRP)
- [ ] 37. Break SRP with a zero key
- [ ] 38. Offline dictionary attack on simplified SRP
- [x] 39. Implement RSA
- [x] 40. Implement an E=3 RSA Broadcast attack

**Set 6 — RSA and DSA**
- [ ] 41. Implement unpadded message recovery oracle
- [ ] 42. Bleichenbacher's e=3 RSA Attack
- [ ] 43. DSA key recovery from nonce
- [ ] 44. DSA nonce recovery from repeated nonce
- [ ] 45. DSA parameter tampering
- [x] 46. RSA parity oracle
- [ ] 47. Bleichenbacher's PKCS 1.5 Padding Oracle (Simple Case)
- [ ] 48. Bleichenbacher's PKCS 1.5 Padding Oracle (Complete Case)

## Notes

**Challenge 1.** Hex decoding pairs nibbles into bytes; base64 encoding regroups
three bytes into four six-bit fields. The padding cases are the interesting part —
one leftover byte yields two characters and `==`, two leftover bytes yield three
characters and `=`.

**Challenge 2.** Fixed XOR over equal-length buffers. The rule throughout is to
operate on raw bytes and treat hex only as a display format.

**Challenge 3.** Try all 256 single-byte keys and score each output by English letter
frequency, with space weighted highest (13) because it is the most common character in
English text. The best score is key `0x58` ('X'): "Cooking MC's like a pound of bacon".

**Challenge 4.** The same search run on each of the 327 lines in the file, keeping the
best-scoring decryption across all of them. Line 171 decrypts with key `0x35` ('5') to
"Now that the party is jumping". Every other line is random bytes, so its best key still
scores far below real English.

**Challenge 5.** Repeating-key XOR: plaintext byte `i` is XORed with key byte `i mod 3`
under the key "ICE", then hex-encoded and compared against the published ciphertext.
The plaintext is two lines, so the newline (`0x0a`) between them has to be in the input
for the output to match.

**Challenge 6.** Breaking repeating-key XOR. The file is base64-decoded, then each
candidate key length from 2 to 40 is scored by splitting the ciphertext into that many
columns and averaging each column's index of coincidence over all 256 byte values.
Each column was encrypted with a single key byte, so the correct length gives the
least uniform columns and the highest score. This replaces the challenge's suggested
Hamming-distance test. The best length is 29; each column is then solved with the
challenge 3 single-byte search, which recovers the key "Terminator X: Bring the noise".

**Challenge 7.** AES-128 in ECB mode under the key "YELLOW SUBMARINE": base64-decode the
file, expand the key once, and run my `aes_decrypt_block` on each 16-byte block, then strip
the PKCS#7 padding (the last byte gives the pad length, 1 to 16). ECB encrypts each block
independently with the same key, so there is no IV. The output is byte-for-byte identical to the
earlier OpenSSL (`EVP_aes_128_ecb`) version.

**Challenge 8.** Detecting ECB. Because ECB is deterministic, two identical 16-byte
plaintext blocks always produce identical ciphertext blocks. Each hex line is split into
32-character (16-byte) blocks and checked for a repeated block. Line 133 of 204 is the
only one with repeats, so it is the ECB ciphertext.

**Challenge 39.** Textbook RSA with e = 3. Key generation picks two random primes of the
requested size (a random number with its top bit set, then GMP's `mpz_nextprime`), and
picks again until p ≠ q and e is coprime to (p − 1)(q − 1). `invmod` is the extended
Euclidean algorithm, and `ModPower` is square-and-multiply. A message is read as one
big-endian number, encrypted, decrypted, and checked against the original.

**Challenge 40.** The e = 3 broadcast attack. The same message is encrypted under three
(or more) public keys, then combined with the Chinese Remainder Theorem into one number
mod n₀·n₁·n₂. Because m³ is smaller than that product, the result is m³ exactly, so an
integer cube root (binary search) recovers the message without any private key.

**Challenge 46.** RSA parity oracle with a 1024-bit modulus. Multiplying the ciphertext by
2^e mod n doubles the plaintext, and n is odd, so the parity of the doubled plaintext says
whether it wrapped past n. Each oracle answer halves the interval [0, n]; the bounds are kept
as exact fractions (`mpq_class`) so no rounding creeps in. After log₂(n) steps the upper bound
is the message, and the common prefix of the two bounds prints as it decrypts.
