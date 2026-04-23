#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>

typedef struct s_args {
    char*   command;
    char**  argv;
    int     argc;
} t_args;

typedef enum {
    INPUT_STDIN,
    INPUT_FILE,
    INPUT_STRING
} t_input_type;

int  parse_args(int argc, char** argv, t_args* arguments);

#endif