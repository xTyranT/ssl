#ifndef SHA256_H
#define SHA256_H

#include "core/dispatcher.h"

typedef struct s_sha256_cmd {
    t_flags flags;
    t_hash_input* input;
    size_t input_count;
    size_t input_capacity;
} t_sha256_cmd;

int cmd_sha256(t_args* args);

#endif