#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>


typedef enum {
    INPUT_STDIN,
    INPUT_FILE,
    INPUT_STRING
} t_input_type;

typedef struct {
    char*        command;
    int          flag;
    char*        input;
    t_input_type input_type;
} t_args;

t_args  parse_args(int argc, char** argv);

#endif