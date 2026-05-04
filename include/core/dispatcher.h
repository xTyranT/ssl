#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "parser.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    const char* name;
    int         (*fn)(t_args *args);
} t_command;

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

int dispatch(t_args* args);

#endif