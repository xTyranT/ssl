# ft_ssl

A partial reimplementation of OpenSSL's command-line tool in C, built as part of the 42 curriculum. Supports message digest (hashing) and encoding (cipher) commands with a modular, extensible architecture.

---

## Table of Contents

- [Overview](#overview)
- [Supported Commands](#supported-commands)
- [Installation](#installation)
- [Usage](#usage)
- [Flags](#flags)
- [Examples](#examples)
- [Architecture](#architecture)
- [Adding a New Algorithm](#adding-a-new-algorithm)
- [Project Structure](#project-structure)

---

## Overview

`ft_ssl` mimics the behaviour of `openssl dgst` and `openssl enc` for a subset of algorithms. It reads input from a string (`-s`), one or more files, or standard input, and prints the computed digest or encoding to standard output.

---

## Supported Commands

| Category | Command     | Description              |
|----------|-------------|--------------------------|
| Hash     | `md5`       | MD5 message digest       |
| Hash     | `sha256`    | SHA-256 message digest   |
| Hash     | `whirlpool` | Whirlpool message digest |
| Cipher   | `base64`    | Base64 encoding          |

---

## Installation

```bash
git clone https://github.com/youruser/ft_ssl.git
cd ft_ssl
make
```

The compiled binary is placed in the project root as `ft_ssl`.

To remove object files and the binary:

```bash
make fclean
```

---

## Usage

**Digest commands (md5, sha256, whirlpool):**
```
./ft_ssl <command> [-pqr] [-s string] [file ...]
```

**Cipher commands (base64):**
```
./ft_ssl base64 [-e|-d] [-i inputfile] [-o outputfile]
```

If no input file is given and `-s` is not used, `ft_ssl` reads from standard input until EOF (`Ctrl+D`).
If no output file is given, results are written to standard output.

---

## Flags

### Digest flags — `md5`, `sha256`, `whirlpool`

| Flag       | Description                                     |
|------------|-------------------------------------------------|
| `-p`       | Echo stdin to stdout before printing the digest |
| `-q`       | Quiet mode — print only the hash, no label      |
| `-r`       | Reverse format — hash first, then filename      |
| `-s <str>` | Hash the given string instead of a file         |

### Cipher flags — `base64`

| Flag          | Description                                        |
|---------------|----------------------------------------------------|
| `-e`          | Encode input to Base64 (default if neither set)    |
| `-d`          | Decode Base64 input back to raw bytes              |
| `-i <file>`   | Read input from file instead of stdin              |
| `-o <file>`   | Write output to file instead of stdout             |

---

## Examples

**Hash a string:**
```bash
./ft_ssl md5 -s "hello world"
MD5 ("hello world") = 5eb63bbbe01eeed093cb22bb8f5acdc3
```

**Hash a file:**
```bash
./ft_ssl sha256 file.txt
SHA256 (file.txt) = 2cf24dba5fb0a30e...
```

**Hash multiple files:**
```bash
./ft_ssl md5 file1.txt file2.txt
```

**Read from stdin:**
```bash
echo -n "hello" | ./ft_ssl whirlpool
```

**Echo stdin and print digest (`-p`):**
```bash
echo "hello" | ./ft_ssl md5 -p
hello
MD5 ("hello") = ...
```

**Quiet mode (`-q`):**
```bash
./ft_ssl sha256 -q -s "hello"
2cf24dba5fb0a30e...
```

**Reverse format (`-r`):**
```bash
./ft_ssl md5 -r file.txt
5eb63bbbe01eeed... file.txt
```

**Base64 encode stdin (default):**
```bash
echo -n "hello world" | ./ft_ssl base64 -e
aGVsbG8gd29ybGQ=
```

**Base64 encode a file:**
```bash
./ft_ssl base64 -e -i input.txt
```

**Base64 encode a file and write to output file:**
```bash
./ft_ssl base64 -e -i input.txt -o input.b64
```

**Base64 decode a file:**
```bash
./ft_ssl base64 -d -i input.b64
hello world
```

**Base64 decode and write result to file:**
```bash
./ft_ssl base64 -d -i input.b64 -o decoded.txt
```

---

## Architecture

```
CLI Input
   ↓
main.c
   ↓
core/parser.c       ← parse flags, command, input source
   ↓
core/dispatcher.c   ← look up handler via function table
   ↓
io/input.c          ← read from stdin / file / string
   ↓
hash/*.c | cipher/*.c   ← compute digest or encoding
   ↓
io/output.c         ← format and print result
```

**Key design decisions:**

- **Function dispatch table** — `dispatcher.c` maps command names to handler function pointers. No `if/else` chains; adding a new algorithm is a one-line change.
- **Separation of concerns** — parsing, I/O, and algorithm logic are fully decoupled. Each layer has a single responsibility.
- **Uniform handler signature** — every algorithm exposes a `*_handler()` function with the same signature, so the dispatcher can call any of them identically.

---

## Adding a New Algorithm

1. Create `include/hash/newAlgo.h` (or `include/cipher/newAlgo.h`)
2. Create `src/hash/newAlgo.c` (or `src/cipher/newAlgo.c`) implementing `newAlgo_handler()`
3. Add a row to the `commands[]` table in `src/core/dispatcher.c`
4. Include the header in `include/ft_ssl.h`

No changes are needed in `parser.c`, `input.c`, `output.c`, or `main.c`.

---

## Project Structure

```
ssl/
├── include/
│   ├── ft_ssl.h
│   ├── core/
│   │   ├── dispatcher.h
│   │   └── parser.h
│   ├── hash/
│   │   ├── md5.h
│   │   ├── sha256.h
│   │   └── whirlpool.h
│   ├── cipher/
│   │   └── base64.h
│   └── io/
│       ├── input.h
│       └── output.h
│
├── src/
│   ├── main.c
│   ├── core/
│   │   ├── dispatcher.c
│   │   └── parser.c
│   ├── hash/
│   │   ├── md5.c
│   │   ├── sha256.c
│   │   └── whirlpool.c
│   ├── cipher/
│   │   └── base64.c
│   └── io/
│       ├── input.c
│       └── output.c
│
├── utils.c
├── Makefile
└── README.md
```

---

## Notes

- Behaviour is designed to match `openssl` output as closely as possible for the supported commands.
- Input is fully read into memory before hashing — not suitable for very large files.
- This project is a 42 school exercise; it is not intended for production cryptographic use.