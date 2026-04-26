#include "io/input.h"

char* read_fd(size_t* len, int fd)
{
    char* buffer = NULL;
    char tmp[BUFFER_SIZE];
    size_t capacity = 0, total = 0;
    int n = -1;

    while ((n = read(fd, tmp, BUFFER_SIZE)))
    {
        if (n < 0)
            return free(buffer), perror("ft_ssl: md5"), NULL;
        if (n == 0)
            break;
        if (total + n > capacity) {
            if (capacity == 0)
                capacity = BUFFER_SIZE;
            while (total + n > capacity)
                capacity *= 2;
            buffer = realloc(buffer, capacity);
        }
        memcpy(buffer + total, tmp, n);
        total += n;
    }
    return *len = total, buffer;
}