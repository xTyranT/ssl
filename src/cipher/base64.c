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

static char* base64_encode(char* input)
{
    (void)input;
    return "ENCODE";
}

static char* base64_decode(char* input)
{
    (void)input;
    return "DECODE";
}

static int process_base64_stdin(t_base64_cmd* base64)
{
    char* input = NULL;
    char* result = NULL;
    size_t len = 0;
    input = read_fd(&len, STDIN_FILENO);
    if (!input)
        return 1;
    if (base64->mode == ENCODE)
        result = base64_encode(input);
    else
        result = base64_decode(input);
    if (!result)
        return 1;
    return write_to_fd(STDOUT_FILENO, ft_strlen(result), result);
}

static int process_base64_file(t_base64_cmd* base64)
{
    char* input = NULL;
    char* result = NULL;
    size_t len = 0;
    int fd = open(base64->in_file, O_RDONLY);
    if (fd < 0)
        return perror("open:"), 1;
    input = read_fd(&len, fd);
    if (!input)
        return 1;
    if (base64->mode == ENCODE)
        result = base64_encode(input);
    else
        result = base64_decode(input);
    if (!result)
        return 1;
    // print res
    return 0;
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