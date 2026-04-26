#ifndef PARSER_H
#define PARSER_H

typedef struct s_args {
    char*   command;
    char**  argv;
    int     argc;
} t_args;

int  parse_args(int argc, char** argv, t_args* arguments);

#endif