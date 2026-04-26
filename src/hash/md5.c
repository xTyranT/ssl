#include "hash/md5.h"
#include "utils/utils.h"
#include "io/input.h"

static void initialize_md5(t_md5_cmd* md5)
{
    md5->flags.p = false;
    md5->flags.q = false;
    md5->flags.r = false;
    md5->input_count = 0;
    md5->input = NULL;
}

static int validate_md5_flags(char* flag, t_flags* flags)
{
    if (!ft_strcmp("p", flag))
        return flags->p = true, 0;
    else if (!ft_strcmp("q", flag))
        return flags->q = true, 0;
    else if (!ft_strcmp("r", flag))
        return flags->r = true, 0;
    print_usage();
    return 1;
}

static int parse_md5_args(t_args* args, t_md5_cmd* md5)
{
    md5->input_capacity = args->argc;
    if (md5->input_capacity > 0) {
        md5->input = malloc(sizeof(t_hash_input) * md5->input_capacity);
        if (!md5->input)
            return perror("ft_ssl: md5"), 1;
    }
    
    int i = 0;
    while(args->argv && args->argv[i] && args->argv[i][0] == '-') {
        if (!ft_strcmp(args->argv[i], "-s")) {
            if (!args->argv[i + 1])
                return print_usage(), free(md5->input), 1;
            md5->input_count++;
            md5->input[md5->input_count - 1].type = INPUT_STR;
            md5->input[md5->input_count - 1].value = args->argv[i + 1];
            i+= 2;
            continue;
        }
        if (validate_md5_flags(&args->argv[i][1], &md5->flags))
            return free(md5->input), 1;
        i++;
    }
    while(args->argv && args->argv[i]) {
        md5->input_count++;
        md5->input[md5->input_count - 1].type = INPUT_FILE;
        md5->input[md5->input_count - 1].value = args->argv[i];
        i++;
    }
    return 0;
}

static void print_md5_result(t_md5_cmd* md5, char* hash, t_input_type type, char* source)
{
    if (md5->flags.q == true || type == INPUT_STDIN) {
        write(STDOUT_FILENO, "MD5 (stdin) = ", 14);
        write(STDOUT_FILENO, hash, ft_strlen(hash));
        write(STDOUT_FILENO, "\n", 1);
        return;
    }
    if (md5->flags.r == true) {
        write(STDOUT_FILENO, hash, ft_strlen(hash));
        write(STDOUT_FILENO, " ", 1);
        write(STDOUT_FILENO, source, ft_strlen(source));
        write(STDOUT_FILENO, "\n", 1);
        return;
    }
    else
{
    write(STDOUT_FILENO, "MD5 (", 5);

    if (type == INPUT_STR)
        write(STDOUT_FILENO, "\"", 1);

    write(STDOUT_FILENO, source, ft_strlen(source));

    if (type == INPUT_STR)
        write(STDOUT_FILENO, "\"", 1);

    write(STDOUT_FILENO, ") = ", 4);
    write(STDOUT_FILENO, hash, ft_strlen(hash));
    write(STDOUT_FILENO, "\n", 1);
}
}

static int process_md5_stdin(t_md5_cmd* md5)
{
    size_t len = 0;
    char *input = read_fd(&len, STDIN_FILENO);
    if (!input)
        return 1;
    if (md5->flags.p)
        if (write(STDOUT_FILENO, input, len) < 0)
            return perror("ft_ssl: md5"), 1;
    print_md5_result(md5, "FAKE STDIN HASH!", INPUT_STDIN, input);
    free(input);
    return 0;
}

static int process_md5_string(t_md5_cmd* md5, char* input)
{
    print_md5_result(md5, "FAKE STRING HASH!", INPUT_STR, input);
    return 0;
}

static int process_md5_file(t_md5_cmd* md5, char* filename)
{
    size_t len = 0;
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return perror(filename), 1;
    char* input = read_fd(&len, fd);
    close(fd);
    print_md5_result(md5, "FAKE FILE HASH!", INPUT_FILE, filename);
    free(input);
    return 0;
}

static int process_md5(t_md5_cmd* md5)
{
    int status = 0;
    if (md5->flags.p || !md5->input)
        status = process_md5_stdin(md5);
    size_t i = 0;
    while (i < md5->input_count) {
        if (md5->input[i].type == INPUT_STR)
            status = process_md5_string(md5, md5->input[i].value);
        if (md5->input[i].type == INPUT_FILE)
            status = process_md5_file(md5, md5->input[i].value);
        i++;
    }
    return status;
}

int cmd_md5(t_args* args)
{
    t_md5_cmd md5;
    initialize_md5(&md5);
    if (parse_md5_args(args, &md5))
        return 1;
    int status = process_md5(&md5);
    if (md5.input)
        free(md5.input);
    return status;
}