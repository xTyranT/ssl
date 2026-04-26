#include "utils/utils.h"

int ft_strlen(const char *s)
{
	size_t i;

	i = 0;
	while (s && s[i])
		i++;
	return (i);
}

int ft_strcmp(const char *s1, const char *s2)
{
    int	i = 0;

	if (!s1 || !s2)
		return (-1);
	while (s1[i] || s2[i])
	{
		if ((unsigned char)s1[i] != (unsigned char)s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}

void print_usage(void)
{
	write(STDERR_FILENO, "usage: ft_ssl command [flags] [file/string]\n", 45);
    write(STDERR_FILENO, "Commands:\n", 11);
    write(STDERR_FILENO, "md5\n", 4);
    write(STDERR_FILENO, "sha256\n", 8);
    write(STDERR_FILENO, "whirlpool\n\n", 12);
    write(STDERR_FILENO, "Flags:\n", 8);
    write(STDERR_FILENO, "-p -q -r -s\n", 13);
}