#ifndef OUTPUT_H
#define OUTPUT_H

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

int write_to_fd(int fd, size_t len, char* output);
void base64_error_extra_args(char *arg);
void base64_error_missing_arg(void);
void base64_error_flag(char* flag);
void des_error_extra_args(char *arg);
void des_error_missing_arg(void);
void des_error_flag(char* flag);
void print_usage(void);

#endif