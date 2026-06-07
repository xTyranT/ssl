#include "cipher/base64.h"
#include "utils/utils.h"
#include "io/input.h"
#include "io/output.h"

static void initialize_base64(t_base64_cmd* base64)
{
    base64->mode = ENCODE;
    base64->input_type = BASE64_STDIN;
    base64->in_file = NULL;
    base64->out_file = NULL;
}

static char* base64_encode(char* input, size_t len)
{
    
    static char base64_table[64] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t i = 0, j = 0;
    size_t out_len = ((len + 2) / 3) * 4;
    char* output = malloc(out_len + 1);
    if (!output)
        return perror("malloc: "), NULL;
    while (i < len) {
        output[j++] = base64_table[ (input[i] & 0xFC) >> 2 ];
        if ((i + 1) == len) {
            output[j++] = base64_table[ ((input[i] & 0x3) << 4) ];
            output[j++] = '=';
            output[j++] = '=';
            break;
        }
        output[j++] = base64_table[ ((input[i] & 0x3) << 4) | ((input[i+1] & 0xF0) >> 4) ];
        if ((i + 2) == len) {
            output[j++] = base64_table[ ((input[i + 1] & 0xF) << 2) ];
            output[j++] = '=';
            break;
        }
        output[j++] = base64_table[ ((input[i + 1] & 0xF) << 2) | ((input[i + 2] & 0xC0) >> 6) ];
        output[j++] = base64_table[ input[i + 2] & 0x3F ];
        i += 3;
    }
    output[j] = 0;
    return output;
}

static char* base64_decode(char* input, size_t len)
{
    char* output;
    if (len == 0) {
        output = malloc(1);
        if (!output)
            return (NULL);
        output[0] = '\0';
        return (output);
    }
    if (len % 4 != 0)
        return (write(1, "base64: invalid input\n", 22), NULL);
    static char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t out_len = (len / 4) * 3;
    if (input[len - 1] == '=')
        out_len--;
    if (input[len - 2] == '=')
        out_len--;
    int indexes[len];
    for (size_t i = 0; i < len; i++) {
        int found = 0;
        if (input[i] == '=') {
            indexes[i] = 0;
            continue;
        }
        for (int j = 0; j < 64; j++) {
            if (input[i] == base64_table[j]) {
                indexes[i] = j;
                found = 1;
                break;
            }
        }
        if (!found)
            return (write(1, "base64: invalid input\n", 22), NULL);
    }
    output = malloc(out_len + 1);
    if (!output)
        return (perror("malloc"), NULL);
    size_t i = 0, j = 0;
    while (i < len) {
        int v1 = indexes[i];
        int v2 = indexes[i + 1];
        int v3 = indexes[i + 2];
        int v4 = indexes[i + 3];
        int combined = (v1 << 18) | (v2 << 12) | (v3 << 6) | v4;
        output[j++] = (combined >> 16) & 0xff;
        if (input[i + 2] != '=')
            output[j++] = (combined >> 8) & 0xff;
        if (input[i + 3] != '=')
            output[j++] = combined & 0xff;
        i += 4;
    }
    output[out_len] = '\0';
    return (output);
}

static int process_base64_stdin(t_base64_cmd* base64)
{
    char* input = NULL;
    char* result = NULL;
    size_t len = 0;
    int status = 0;
    input = read_fd(&len, STDIN_FILENO);
    if (!input)
        return 1;
    if (base64->mode == ENCODE)
        result = base64_encode(input, len);
    else
        result = base64_decode(input, len);
    if (!result)
        return free(input), 1;
    if (!base64->out_file)
        status = write_to_fd(STDOUT_FILENO, ft_strlen(result), result);
    else {
        int ofd = open(base64->out_file, O_RDWR);
        status = write_to_fd(ofd, ft_strlen(result), result);
        close(ofd);
    }
    free(result);
    free(input);
    return status;
}

static int process_base64_file(t_base64_cmd* base64)
{
    char* input = NULL;
    char* result = NULL;
    size_t len = 0;
    int status = 0;
    int fd = open(base64->in_file, O_RDONLY);
    if (fd < 0)
        return perror("open:"), 1;
    input = read_fd(&len, fd);
    close(fd);
    if (!input)
        return 1;
    if (base64->mode == ENCODE)
        result = base64_encode(input, len);
    else
        result = base64_decode(input, len);
    if (!result)
        return free(input), 1;
    if (!base64->out_file)
        status = write_to_fd(STDOUT_FILENO, ft_strlen(result), result);
    else {
        int ofd = open(base64->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        status = write_to_fd(ofd, ft_strlen(result), result);
        close(ofd);
    }
    free(result);
    free(input);
    return status;
}

static int process_base64(t_base64_cmd* base64)
{
    if (base64->input_type == BASE64_FILE)
        return process_base64_file(base64);
    return process_base64_stdin(base64);
}

static int validate_base64_flags(char* arg, t_base64_cmd* base64)
{
    if (!ft_strcmp(arg, "-e"))
        base64->mode = ENCODE;
    else if (!ft_strcmp(arg, "-d"))
        base64->mode = DECODE;
    else
        return base64_error_flag(arg), 1;
    return 0;
}

static int parse_base64_args(t_args* args, t_base64_cmd* base64)
{
    int i = 0;
    while (args->argv && args->argv[i] && args->argv[i][0] == '-') {
        if (!ft_strcmp(args->argv[i], "-o") || !ft_strcmp(args->argv[i], "-i")) {
            if (!args->argv[i + 1])
               return base64_error_missing_arg(), 1;
            if (!ft_strcmp(args->argv[i], "-i")) {
                base64->in_file = args->argv[i + 1];
                base64->input_type = BASE64_FILE;
            }
            else if (!ft_strcmp(args->argv[i], "-o"))
                base64->out_file = args->argv[i + 1];
            i += 2;
            continue;
        }
        if (validate_base64_flags(args->argv[i], base64))
            return 1;
        i++;
    }
    if (args->argv[i])
        return base64_error_extra_args(args->argv[i]), 1;
    return 0;
}

int cmd_base64(t_args* args)
{
    t_base64_cmd base64;
    initialize_base64(&base64);
    if (parse_base64_args(args, &base64))
        return 1;
    return process_base64(&base64);
}