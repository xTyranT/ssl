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

static char *md5_hash(char *input, size_t len)
{
    uint32_t s[64] = {
        7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
        5,9,14,20, 5,9,14,20, 5,9,14,20, 5,9,14,20,
        4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
        6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21
    };
    uint32_t K[64] = {
        0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
        0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
        0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,
        0x6b901122,0xfd987193,0xa679438e,0x49b40821,
        0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
        0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
        0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,
        0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
        0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,
        0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
        0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
        0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
        0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,
        0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
        0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
        0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
    };

    size_t new_len = len + 1;
    while (new_len % 64 != 56)
        new_len++;
    new_len += 8;
    unsigned char *msg = malloc(new_len);
    if (!msg)
        return NULL;
    memcpy(msg, input, len);
    msg[len] = 0x80;
    for (size_t i = len + 1; i < new_len; i++)
        msg[i] = 0;

    uint64_t bit_len = (uint64_t)len * 8;
    for (int i = 0; i < 8; i++)
        msg[new_len - 8 + i] = (bit_len >> (8 * i)) & 0xff;

    uint32_t A = 0x67452301;
    uint32_t B = 0xefcdab89;
    uint32_t C = 0x98badcfe;
    uint32_t D = 0x10325476;

    for (size_t offset = 0; offset < new_len; offset += 64) {
        uint32_t M[16];
        for (int i = 0; i < 16; i++) {
            M[i] = (uint32_t)msg[offset + i * 4]
                 | ((uint32_t)msg[offset + i * 4 + 1] << 8)
                 | ((uint32_t)msg[offset + i * 4 + 2] << 16)
                 | ((uint32_t)msg[offset + i * 4 + 3] << 24);
        }
        uint32_t a = A, b = B, c = C, d = D;
        for (int i = 0; i < 64; i++) {
            uint32_t F, g;
            if (i < 16) {
                F = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32) {
                F = (d & b) | ((~d) & c);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48) {
                F = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            }
            else {
                F = c ^ (b | (~d));
                g = (7 * i) % 16;
            }
            uint32_t temp = d;
            d = c;
            c = b;
            uint32_t rotate = a + F + K[i] + M[g];
            b = b + ((rotate << s[i]) | (rotate >> (32 - s[i])));
            a = temp;
        }
        A += a;
        B += b;
        C += c;
        D += d;
    }

    unsigned char digest[16];

    digest[0] = A & 0xff;
    digest[1] = (A >> 8) & 0xff;
    digest[2] = (A >> 16) & 0xff;
    digest[3] = (A >> 24) & 0xff;

    digest[4] = B & 0xff;
    digest[5] = (B >> 8) & 0xff;
    digest[6] = (B >> 16) & 0xff;
    digest[7] = (B >> 24) & 0xff;

    digest[8] = C & 0xff;
    digest[9] = (C >> 8) & 0xff;
    digest[10] = (C >> 16) & 0xff;
    digest[11] = (C >> 24) & 0xff;

    digest[12] = D & 0xff;
    digest[13] = (D >> 8) & 0xff;
    digest[14] = (D >> 16) & 0xff;
    digest[15] = (D >> 24) & 0xff;

    char *out = malloc(33);
    if (!out)
        return NULL;
    char *hex = "0123456789abcdef";
    for (int i = 0; i < 16; i++) {
        out[i * 2] = hex[digest[i] >> 4];
        out[i * 2 + 1] = hex[digest[i] & 0x0f];
    }
    out[32] = '\0';
    free(msg);
    return out;
}

static void print_md5_result(t_md5_cmd* md5, char* hash, t_input_type type, char* source)
{
    if (type == INPUT_STDIN) {
        if (md5->flags.q == false)
            write(STDOUT_FILENO, "MD5 (stdin) = ", 14);
        write(STDOUT_FILENO, hash, ft_strlen(hash));
        write(STDOUT_FILENO, "\n", 1);
        return;
    }
    if (md5->flags.r == true) {
        write(STDOUT_FILENO, hash, ft_strlen(hash));
        if (md5->flags.q == false) {
            write(STDOUT_FILENO, " ", 1);
            write(STDOUT_FILENO, source, ft_strlen(source));
        }
        write(STDOUT_FILENO, "\n", 1);
        return;
    }
    else {
        if (md5->flags.q == false) {
            write(STDOUT_FILENO, "MD5 (", 5);
            if (type == INPUT_STR)
                write(STDOUT_FILENO, "\"", 1);
            write(STDOUT_FILENO, source, ft_strlen(source));
            if (type == INPUT_STR)
                write(STDOUT_FILENO, "\"", 1);
            write(STDOUT_FILENO, ") = ", 4);
        }
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
    char* hash = md5_hash(input, len);
    print_md5_result(md5, hash, INPUT_STDIN, input);
    free(hash);
    free(input);
    return 0;
}

static int process_md5_string(t_md5_cmd* md5, char* input)
{
    char* hash = md5_hash(input, ft_strlen(input));
    print_md5_result(md5, hash, INPUT_STR, input);
    free(hash);
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
    char* hash = md5_hash(input, len);
    print_md5_result(md5, hash, INPUT_FILE, filename);
    free(hash);
    free(input);
    return 0;
}

static int process_md5(t_md5_cmd* md5)
{
    int status = 0;
    if (md5->flags.p || !md5->input_count)
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