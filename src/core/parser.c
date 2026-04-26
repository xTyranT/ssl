#include "core/parser.h"
#include "utils/utils.h"

int parse_args(int argc, char** argv, t_args* arguments)
{
    if (argc < 2) {
        print_usage();
        return 1;
    }
    arguments->command = argv[1];
    arguments->argc = argc - 2;
    arguments->argv = argv + 2;
    return 0;
}