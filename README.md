# ft_ssl

A simplified reimplementation of OpenSSL hashing commands written in C.

This project supports multiple cryptographic hash algorithms with various input types and flags, closely mimicking the behavior of `openssl`.

---

## Features

### Algorithms
- MD5
- SHA-256
- Whirlpool

### Input Types
- Standard input (stdin)
- Files
- Strings (`-s`)

### Flags
- `-p` : echo stdin to stdout before hashing
- `-q` : quiet mode (print only the hash)
- `-r` : reverse format (hash first, then input)
- `-s` : hash a given string

---

## Build

```bash
make
```

Optional debug build:

```bash
make debug
```

---

## Usage

```bash
./ft_ssl <command> [options] [files...]
```

### Examples

**String**
```bash
./ft_ssl md5 -s "test"
# MD5 ("test") = 098f6bcd4621d373cade4e832627b4f6
```

**File**
```bash
./ft_ssl sha256 file.txt
# sha256 (file.txt) = <hash>
```

**Stdin**
```bash
echo "test" | ./ft_ssl md5
# MD5 (stdin) = <hash>
```

**Reverse (`-r`)**
```bash
./ft_ssl md5 -r -s "test"
# 098f6bcd4621d373cade4e832627b4f6 test
```

**Quiet (`-q`)**
```bash
./ft_ssl md5 -q -s "test"
# 098f6bcd4621d373cade4e832627b4f6
```

**Echo stdin (`-p`)**
```bash
echo "test" | ./ft_ssl md5 -p
# test
# MD5 (stdin) = <hash>
```

---

## Project Structure

```
.
├── include/
│   ├── core/
│   ├── hash/
│   ├── io/
│   └── utils/
├── src/
│   ├── core/
│   ├── hash/
│   ├── io/
│   ├── utils/
│   └── main.c
├── tester.sh
├── Makefile
└── README.md
```

---

## Testing

Run all tests:

```bash
./tester.sh
```

or:

```bash
make test
```

Includes:
- Output comparison with OpenSSL
- Edge cases
- Error handling
- Memory leak checks (Valgrind)

---

## Memory Safety

Test with:

```bash
valgrind --leak-check=full ./ft_ssl md5 -s "test"
```

No memory leaks should be present.

---

## Notes

- Output format follows project specifications.
- Whirlpool testing may require manual validation depending on OpenSSL support.

---

## Learning Objectives

- Understand how hash functions work internally
- Work with bitwise operations and data transformations
- Implement CLI parsing in C
- Manage memory safely
- Build scalable and modular architecture

---

## Author

**Kamal Ouferkach**