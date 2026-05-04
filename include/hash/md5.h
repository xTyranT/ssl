#ifndef MD5_H
#define MD5_H

#include "core/dispatcher.h"

typedef struct s_md5_cmd {
    t_flags flags;
    t_hash_input* input;
    size_t input_count;
    size_t input_capacity;
} t_md5_cmd;

int cmd_md5(t_args* args);

#endif