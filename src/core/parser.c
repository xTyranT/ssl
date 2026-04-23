#include "core/parser.h"

t_args  parse_args(int argc, char** argv)
{
    t_args arguments;
    if (argc < 2) {
        fprintf(stderr, "usage: ft_ssl command [flags] [file/string]\n");
        arguments.command = NULL;
        return arguments;
    }
    
}