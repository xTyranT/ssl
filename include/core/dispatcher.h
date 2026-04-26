#ifndef DISPATCHER_H
#define DISPATCHER_H

#include "parser.h"

#include <stdio.h>

typedef struct {
    const char* name;
    int         (*fn)(t_args *args);
} t_command;

int dispatch(t_args* args);

#endif