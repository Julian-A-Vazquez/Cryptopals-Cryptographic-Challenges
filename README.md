# Cryptopals

Solutions to the [Cryptopals challenges](https://cryptopals.com), in C++.
Sets 2–7 on the above link. Set 8 (the abstract algebra set) is not published on the
main site but instead here: (https://gist.github.com/arkadiyt/5b33bed653ce1dc26e1df9c249d8919e).

Cryptographic primitives are implemented from scratch rather than pulled from
a library. The exception so far is challenge 7, which uses OpenSSL's AES-128-ECB as the
challenge suggests; the plan is to swap in my own [FIPS 197 implementation](https://github.com/Julian-A-Vazquez/aes-128-fips197)
once it's complete.

## Layout

One directory per set. Each solution is self-contained and carries its own `main`
with the challenge's published input, and prints either `PASS` or the recovered key and plaintext.

```
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

Challenge 7 links against OpenSSL's libcrypto:

```
g++ -std=c++17 -O2 -Wall -Wextra -o challenge set1/Set_1_Challenge_7.cpp -lcrypto
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
file, then decrypt with OpenSSL's EVP interface (`EVP_aes_128_ecb`). ECB encrypts each
16-byte block independently with the same key, so there is no IV.

**Challenge 8.** Detecting ECB. Because ECB is deterministic, two identical 16-byte
plaintext blocks always produce identical ciphertext blocks. Each hex line is split into
32-character (16-byte) blocks and checked for a repeated block. Line 133 of 204 is the
only one with repeats, so it is the ECB ciphertext.
