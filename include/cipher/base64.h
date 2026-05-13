#ifndef BASE64_H
#define BASE64_H

#include "core/dispatcher.h"

typedef enum e_base64_mode {
    ENCODE,
    DECODE
} t_base64_mode;

typedef enum e_base64_input_type {
    BASE64_STDIN,
    BASE64_FILE
} t_base64_input_type;

typedef struct s_base64_cmd {
    t_base64_mode mode;
    t_base64_input_type input_type;
    char* in_file;
    char* out_file;
} t_base64_cmd;

int cmd_base64(t_args* args);

#endif