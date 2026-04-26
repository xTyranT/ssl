#ifndef MD5_H
#define MD5_H

#include "core/dispatcher.h"
#include <stdbool.h>
#include <stdlib.h>

typedef struct s_flags {
    bool p;
    bool q;
    bool r;
} t_flags;

typedef enum e_input_type {
    INPUT_STDIN,
    INPUT_STR,
    INPUT_FILE
} t_input_type;

typedef struct s_hash_input {
    t_input_type type;
    char* value;
} t_hash_input;

typedef struct s_md5_cmd {
    t_flags flags;
    t_hash_input* input;
    size_t input_count;
    size_t input_capacity;
} t_md5_cmd;

int cmd_md5(t_args* args);

#endif