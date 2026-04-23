#include "core/parser.h"

int parse_args(int argc, char** argv, t_args* arguments)
{
    if (argc < 2) {
        fprintf(stderr, "usage: ft_ssl command [flags] [file/string]\n");
        fprintf(stderr, "Commands:\n");
        fprintf(stderr, "md5\n");
        fprintf(stderr, "sha256\n\n");
        fprintf(stderr, "Flags:\n");
        fprintf(stderr, "-p -q -r -s\n");
        return 1;
    }
    arguments->command = argv[1];
    arguments->argc = argc - 2;
    arguments->argv = argv + 2;
    return 0;
}