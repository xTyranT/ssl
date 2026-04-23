#include "core/dispatcher.h"
#include "utils/utils.h"

int cmd_md5(t_args* args)
{
    printf("md\n");
    (void)args;
    return 0;
}
int cmd_whirlpool(t_args* args)
{
    printf("whirlpool\n");
    (void)args;
    return 0;
}
int cmd_sha256(t_args* args)
{
    printf("sha\n");
    (void)args;
    return 0;
}

static const t_command g_commands[] = {
    {"md5", &cmd_md5},
    {"sha256", &cmd_sha256},
    {"whirlpool", &cmd_whirlpool},
    {NULL, NULL}
};
int dispatch(t_args* args)
{
    int i = -1;

    while (g_commands[++i].name) {
        if (ft_strcmp(args->command, g_commands[i].name) == 0) {
            return g_commands[i].fn(args);
        }
    }
    fprintf(stderr, "usage: ft_ssl command [flags] [file/string]\n");
    fprintf(stderr, "Commands:\n");
    fprintf(stderr, "md5\n");
    fprintf(stderr, "sha256\n\n");
    fprintf(stderr, "Flags:\n");
    fprintf(stderr, "-p -q -r -s\n");
    return 1;
}