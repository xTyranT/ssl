#ifndef INPUT_H
#define INPUT_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

char* read_fd(size_t* len, int fd);

#endif