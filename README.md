# ft_ssl

A lightweight, modular implementation of common cryptographic hash functions in C.

## Features

- **MD5** - Message Digest Algorithm 5
- **SHA1** - Secure Hash Algorithm 1
- **SHA256** - Secure Hash Algorithm 256-bit

## Supported Flags

- `-p` - Echo input and hash output
- `-q` - Quiet mode
- `-r` - Reverse output format
- `-s` - Process string argument instead of file

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
./ft_ssl sha1 -r myfile.txt
```

## Project Structure

See [ARCHITECTURE.md](ARCHITECTURE.md) for detailed structure and module descriptions.