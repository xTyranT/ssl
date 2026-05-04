#ifndef WHIRLPOOL_H
#define WHIRLPOOL_H

#include "core/dispatcher.h"

typedef struct s_whirlpool_cmd {
    t_flags flags;
    t_hash_input* input;
    size_t input_count;
    size_t input_capacity;
} t_whirlpool_cmd;

int cmd_whirlpool(t_args* args);

#endif