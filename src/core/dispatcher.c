#include "core/dispatcher.h"
#include "utils/utils.h"
#include "hash/md5.h"
#include "hash/sha256.h"
#include "hash/whirlpool.h"

static const t_command g_commands[] = {
    {"md5", &cmd_md5},
    {"sha256", &cmd_sha256},
    {"whirlpool", &cmd_whirlpool},
    {NULL, NULL}
};

int dispatch(t_args* args)
{
    int i = 0;

    while (g_commands[i].name) {
        if (ft_strcmp(args->command, g_commands[i].name) == 0) {
            return g_commands[i].fn(args);
        }
        i++;
    }
    print_usage();
    return 1;
}