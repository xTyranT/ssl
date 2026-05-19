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

void	print_usage(void)
{
	write(STDERR_FILENO,
		"usage: ft_ssl command [command opts] [command args]\n", 52);
	write(STDERR_FILENO, "\nMessage Digest commands:\n", 26);
	write(STDERR_FILENO, "md5\n", 4);
	write(STDERR_FILENO, "sha256\n", 7);
	write(STDERR_FILENO, "whirlpool\n", 10);
	write(STDERR_FILENO, "\nCipher commands:\n", 18);
	write(STDERR_FILENO, "base64\n", 7);
	write(STDERR_FILENO, "\nDigest flags:\n", 15);
	write(STDERR_FILENO, "-p  echo stdin before hashing\n", 30);
	write(STDERR_FILENO, "-q  quiet mode\n", 15);
	write(STDERR_FILENO, "-r  reverse output format\n", 26);
	write(STDERR_FILENO, "-s  hash string\n", 16);
	write(STDERR_FILENO, "\nBase64 flags:\n", 15);
	write(STDERR_FILENO, "-e  encode mode (default)\n", 26);
	write(STDERR_FILENO, "-d  decode mode\n", 16);
	write(STDERR_FILENO, "-i  input file\n", 15);
	write(STDERR_FILENO, "-o  output file\n", 16);
}