#include "io/output.h"

int write_to_fd(int fd, size_t len, char* output)
{
    if (write(fd, output, len) < 0)
        return perror("write: "), 1;
    return 0;
}

void base64_error_flag(char* flag)
{
    fprintf(stderr, "ft_ssl: base64: unknown option: -%s\n", flag);
    fprintf(stderr, "usage: ft_ssl base64 [-e|-d] [-i inputfile] [-o outputfile]\n");
}

void base64_error_missing_arg(void)
{
    fprintf(stderr, "ft_ssl: base64: option [-o|-i] requires an argument\n");
    fprintf(stderr, "usage: ft_ssl base64 [-e|-d] [-i inputfile] [-o outputfile]\n");
}

void base64_error_extra_args(char *arg)
{
    fprintf(stderr, "ft_ssl: base64: extra argument given: %s\n", arg);
    fprintf(stderr, "usage: ft_ssl base64 [-e|-d] [-i inputfile] [-o outputfile]\n");
}