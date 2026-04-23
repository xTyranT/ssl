# ft_ssl Architecture

## Overview

ft_ssl is organized using a modular, layered architecture that separates concerns and makes the codebase easy to maintain and extend.

## Directory Structure

```
ssl/
├── include/                 # Header files
│   ├── ft_ssl.h            # Main header (includes all modules)
│   ├── core/               # Core functionality
│   │   ├── dispatcher.h    # Command routing
│   │   └── parser.h        # Argument parsing
│   ├── hash/               # Hash algorithms
│   │   ├── md5.h
│   │   ├── whirlpool.h
│   │   └── sha256.h
│   └── io/                 # Input/Output
│       ├── input.h         # Reading files and stdin
│       └── output.h        # Printing results and errors
│
├── src/                     # Implementation files
│   ├── main.c              # Entry point
│   ├── core/
│   │   ├── dispatcher.c    # Command dispatch table
│   │   └── parser.c        # Parse CLI arguments
│   ├── hash/
│   │   ├── md5.c           # MD5 implementation
│   │   ├── whirlpool.c          # whirlpool implementation
│   │   └── sha256.c        # SHA256 implementation
│   └── io/
│       ├── input.c         # File/stdin reading
│       └── output.c        # Error/result printing
│
├── utils.c                  # Utility functions
├── Makefile                 # Build configuration
└── README.md               # Project overview
```

## Module Descriptions

### core/dispatcher.h + dispatcher.c
**Purpose:** Route commands to appropriate hash handlers

- Maintains a function pointer table of supported commands
- Avoids if/else chains through function dispatch pattern
- Easy to add new hash algorithms

**Key Functions:**
- `get_command_handler()` - Returns function pointer for command

### core/parser.h + parser.c
**Purpose:** Parse command-line arguments

- Extracts command name
- Identifies input source (file, string via -s, or stdin)
- Parses flags (-p, -q, -r, -s)
- Validates input

**Key Functions:**
- `parse_args()` - Main parsing function

### io/input.h + input.c
**Purpose:** Handle input from multiple sources

- Read from stdin (until EOF)
- Read from files
- Handle errors gracefully

**Key Functions:**
- `read_stdin()` - Read all of stdin into memory
- `read_file()` - Read entire file into memory

### io/output.h + output.c
**Purpose:** Display usage, errors, and results

- Usage messages
- Error messages with help text
- Formatted output

**Key Functions:**
- `print_usage()` - Show usage message
- `print_error()` - Show error with available commands

### hash/md5.h, hash/whirlpool.h, hash/sha256.h
**Purpose:** Implement individual hash algorithms

- Each module contains the algorithm implementation
- Handler function signature matches dispatcher requirements
- Returns computed hash

**Key Functions:**
- `md5_handler()`, `whirlpool_handler()`, `sha256_handler()` - Hash handlers

## Data Flow

```
CLI Input
   ↓
main.c
   ↓
parser.c (parse arguments)
   ↓
dispatcher.c (select handler)
   ↓
input.c (read data)
   ↓
hash/md5.c|whirlpool.c|sha256.c (compute hash)
   ↓
output.c (format & display result)
```

## Design Patterns Used

1. **Function Dispatch Pattern** - Command routing without conditionals
2. **Separation of Concerns** - Each module has single responsibility
3. **Error Propagation** - Errors handled at each layer

## Extension Points

To add a new hash algorithm:

1. Create `include/hash/newHash.h`
2. Create `src/hash/newHash.c` with `newHash_handler()` function
3. Add entry to `commands[]` table in `dispatcher.c`
4. Include header in `include/ft_ssl.h`

No changes needed to parser, input, output, or main!

## Build System

Makefile features:
- Recursive compilation of nested directories
- Loading spinner during compilation
- Clean separation of object files
