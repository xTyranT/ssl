#ifndef DES_H
#define DES_H

#include "core/dispatcher.h"

typedef enum e_des_mode {
    ENCRYPT,
    DECRYPT
} t_des_mode;

typedef enum e_des_input_type {
    DES_STDIN,
    DES_FILE
} t_des_input_type;

typedef enum e_des_cipher_mode {
    DES_MODE_ECB,
    DES_MODE_CBC,
    DES_MODE_CFB,
    DES_MODE_OFB,
    DES_MODE_CTR
} t_des_cipher_mode;

typedef struct s_des_cmd {
    t_des_mode mode;
    char* in_file;
    char* out_file;
    bool  base64;
    char* key;
    bool  key_generated;
    char* password;
    bool  p_flag;
    char* salt;
    char* vector;
    t_des_cipher_mode mode_type;
    t_des_input_type input_type;
} t_des_cmd;

int cmd_des(t_args* args);

#endif