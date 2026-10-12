# Cryptopals

Solutions to the [Cryptopals challenges](https://cryptopals.com), in C++.
Sets 2–7 on the above link. Set 8 (the abstract algebra set) is not published on the
main site but instead here: (https://gist.github.com/arkadiyt/5b33bed653ce1dc26e1df9c249d8919e).

Cryptographic primitives are implemented from scratch rather than pulled from
a library — AES comes from my [FIPS 197 implementation](https://github.com/Julian-A-Vazquez/aes-128-fips197).

## Layout

One directory per set. Each solution is self-contained and carries its own `main`
with the challenge's published input, and prints either `PASS` or the recovered key and plaintext.

```
set1/Set_1_Challenge_1.cpp
set1/Set_1_Challenge_2.cpp
set1/Set_1_Challenge_3.cpp
set1/Set_1_Challenge_4.cpp
set1/Challenge 4 text.txt     (input for challenge 4)
...
```

## Build

Each file compiles on its own:

```
g++ -std=c++17 -O2 -Wall -Wextra -o challenge set1/Set_1_Challenge_1.cpp
./challenge
```

Challenge 4 opens `Challenge 4 text.txt` from the current directory, so run it from `set1/`.

## Progress

**Set 1 — Basics**
- [x] 1. Convert hex to base64
- [x] 2. Fixed XOR
- [x] 3. Single-byte XOR cipher
- [x] 4. Detect single-character XOR
- [ ] 5. Implement repeating-key XOR
- [ ] 6. Break repeating-key XOR
- [ ] 7. AES in ECB mode
- [ ] 8. Detect AES in ECB mode

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
