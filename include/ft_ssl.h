#ifndef FT_SSL_H
#define FT_SSL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "core/dispatcher.h"
#include "core/parser.h"
#include "hash/md5.h"
#include "hash/whirlpool.h"
#include "hash/sha256.h"
#include "io/input.h"
#include "io/output.h"

void    print_usage(void);

#endif