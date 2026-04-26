# ft_ssl

A lightweight, modular C implementation of common cryptographic hash functions.

## Features

- **MD5** — Message Digest Algorithm 5
- **SHA-256** — Secure Hash Algorithm, 256-bit
- **Whirlpool** — 512-bit cryptographic hash function

## Supported Flags

- `-p` — Read from standard input, echo input, then print the hash
- `-q` — Quiet mode; print only the hash
- `-r` — Reverse output format
- `-s` — Hash the provided string argument instead of a file

## Building

```bash
make        # Build the project
make clean  # Remove object files
make fclean # Remove object files and executable
make re     # Rebuild from scratch
```

## Usage

```bash
# Hash from stdin
echo "hello" | ./ft_ssl md5

# Hash a string
./ft_ssl md5 -s "hello"

# Hash a file
./ft_ssl sha256 myfile.txt

# With flags
./ft_ssl md5 -p -s "hello"
./ft_ssl whirlpool -r myfile.txt
```

## Project Structure

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed structure and module descriptions.
