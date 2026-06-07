#include "cipher/des.h"
#include "utils/utils.h"
#include "io/input.h"
#include "io/output.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

static void initialize_des(t_des_cmd* des)
{
    des->mode = ENCRYPT;
    des->input_type = DES_STDIN;
    des->in_file = NULL;
    des->out_file = NULL;
    des->base64 = false;
    des->key = NULL;
    des->password = NULL;
    des->salt = NULL;
    des->vector = NULL;
}

static int validate_des_flags(char* arg, t_des_cmd* des)
{
    if (!ft_strcmp(arg, "-e"))
        des->mode = ENCRYPT;
    else if (!ft_strcmp(arg, "-d"))
        des->mode = DECRYPT;
    else if (!ft_strcmp(arg, "-a"))
        des->base64 = true;
    else
        return des_error_flag(arg), 1;
    return 0;
}

static int parse_des_args(t_args* args, t_des_cmd* des)
{
    int i = 0;
    while (args->argv && args->argv[i] && args->argv[i][0] == '-') {
        if (!ft_strcmp(args->argv[i], "-o") || !ft_strcmp(args->argv[i], "-i") 
         || !ft_strcmp(args->argv[i], "-k") || !ft_strcmp(args->argv[i], "-p")
         || !ft_strcmp(args->argv[i], "-s") || !ft_strcmp(args->argv[i], "-v")) {
            if (!args->argv[i + 1])
               return des_error_missing_arg(), 1;
            if (!ft_strcmp(args->argv[i], "-i")) {
                des->in_file = args->argv[i + 1];
                des->input_type = DES_FILE;
            }
            else if (!ft_strcmp(args->argv[i], "-o"))
                des->out_file = args->argv[i + 1];
            else if (!ft_strcmp(args->argv[i], "-p"))
                des->password = args->argv[i + 1];
            else if (!ft_strcmp(args->argv[i], "-k"))
                des->key = args->argv[i + 1];
            else if (!ft_strcmp(args->argv[i], "-s"))
                des->salt = args->argv[i + 1];
            else if (!ft_strcmp(args->argv[i], "-v"))
                des->vector = args->argv[i + 1];
            i += 2;
            continue;
        }
        if (validate_des_flags(args->argv[i], des))
            return 1;
        i++;
    }
    if (args->argv[i])
        return des_error_extra_args(args->argv[i]), 1;
    return 0;
}

static int check_hex_string(char* str)
{
    if (!str)
        return 0;
    for (size_t i = 0; str[i]; i++) {
        if (!((str[i] >= '0' && str[i] <= '9') || (str[i] >= 'a' && str[i] <= 'f') || (str[i] >= 'A' && str[i] <= 'F')))
            return fprintf(stderr, "ft_ssl: des: invalid hex string: %s\n", str), 1;
    }
    return 0;
}

static int validate_des_args(t_des_cmd* des)
{
    if (!des->key && !des->password)
        return fprintf(stderr, "ft_ssl: des: either key or password must be provided\n"), 1;
    if (des->key && des->password)
        return fprintf(stderr, "ft_ssl: des: both key and password cannot be provided\n"), 1;
    if (des->key && ft_strlen(des->key) != 16)
        return fprintf(stderr, "ft_ssl: des: key must be 16 hex characters (64 bits)\n"), 1;
    if (des->salt && ft_strlen(des->salt) != 16)
        return fprintf(stderr, "ft_ssl: des: salt must be 16 hex characters (64 bits)\n"), 1;
    if (des->password && ft_strlen(des->password) != 16)
        return fprintf(stderr, "ft_ssl: des: password must be 16 hex characters (64 bits)\n"), 1;
    if (des->vector)
        return fprintf(stderr, "ft_ssl: des: initialization vector is not supported in ECB mode\n"), 1;
    if (check_hex_string(des->key))
        return 1;
    if (check_hex_string(des->salt))
        return 1;
    if (check_hex_string(des->password))
        return 1;
    return 0;
}

static int process_des(t_des_cmd* des)
{
    printf("DES_ECB: mode=%s, input=%s, output=%s, base64=%d, key=%s, password=%s, salt=%s, vector=%s\n",
        des->mode == ENCRYPT ? "encrypt" : "decrypt",
        des->input_type == DES_STDIN ? "stdin" : des->in_file,
        des->out_file ? des->out_file : "stdout",
        des->base64,
        des->key ? des->key : "NULL",
        des->password ? des->password : "NULL",
        des->salt ? des->salt : "NULL",
        des->vector ? des->vector : "NULL");
        return 0;
    // if (des->input_type == DES_FILE)
    //     return process_des_file(des);
    // return process_des_stdin(des);
}

int cmd_des(t_args* args)
{
    t_des_cmd des;
    initialize_des(&des);
    if (parse_des_args(args, &des))
        return 1;
    if (validate_des_args(&des))
        return 1;
    return process_des(&des);
}