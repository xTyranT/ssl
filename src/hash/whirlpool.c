#include "hash/whirlpool.h"
#include "utils/utils.h"
#include "io/input.h"

static void initialize_whirlpool(t_whirlpool_cmd* whirlpool)
{
    whirlpool->flags.p = false;
    whirlpool->flags.q = false;
    whirlpool->flags.r = false;
    whirlpool->input_count = 0;
    whirlpool->input = NULL;
}

static int validate_whirlpool_flags(char* flag, t_flags* flags)
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

static int parse_whirlpool_args(t_args* args, t_whirlpool_cmd* whirlpool)
{
    whirlpool->input_capacity = args->argc;
    if (whirlpool->input_capacity > 0) {
        whirlpool->input = malloc(sizeof(t_hash_input) * whirlpool->input_capacity);
        if (!whirlpool->input)
            return perror("ft_ssl: whirlpool"), 1;
    }
    
    int i = 0;
    while(args->argv && args->argv[i] && args->argv[i][0] == '-') {
        if (!ft_strcmp(args->argv[i], "-s")) {
            if (!args->argv[i + 1])
                return print_usage(), free(whirlpool->input), 1;
            whirlpool->input_count++;
            whirlpool->input[whirlpool->input_count - 1].type = INPUT_STR;
            whirlpool->input[whirlpool->input_count - 1].value = args->argv[i + 1];
            i+= 2;
            continue;
        }
        if (validate_whirlpool_flags(&args->argv[i][1], &whirlpool->flags))
            return free(whirlpool->input), 1;
        i++;
    }
    while(args->argv && args->argv[i]) {
        whirlpool->input_count++;
        whirlpool->input[whirlpool->input_count - 1].type = INPUT_FILE;
        whirlpool->input[whirlpool->input_count - 1].value = args->argv[i];
        i++;
    }
    return 0;
}

static char* whirlpool_hash(char *input, size_t len)
{
    static const unsigned char S[256] = {
        0x18,0x23,0xc6,0xe8,0x87,0xb8,0x01,0x4f,0x36,0xa6,0xd2,0xf5,0x79,0x6f,0x91,0x52,
        0x60,0xbc,0x9b,0x8e,0xa3,0x0c,0x7b,0x35,0x1d,0xe0,0xd7,0xc2,0x2e,0x4b,0xfe,0x57,
        0x15,0x77,0x37,0xe5,0x9f,0xf0,0x4a,0xda,0x58,0xc9,0x29,0x0a,0xb1,0xa0,0x6b,0x85,
        0xbd,0x5d,0x10,0xf4,0xcb,0x3e,0x05,0x67,0xe4,0x27,0x41,0x8b,0xa7,0x7d,0x95,0xd8,
        0xfb,0xee,0x7c,0x66,0xdd,0x17,0x47,0x9e,0xca,0x2d,0xbf,0x07,0xad,0x5a,0x83,0x33,
        0x63,0x02,0xaa,0x71,0xc8,0x19,0x49,0xd9,0xf2,0xe3,0x5b,0x88,0x9a,0x26,0x32,0xb0,
        0xe9,0x0f,0xd5,0x80,0xbe,0xcd,0x34,0x48,0xff,0x7a,0x90,0x5f,0x20,0x68,0x1a,0xae,
        0xb4,0x54,0x93,0x22,0x64,0xf1,0x73,0x12,0x40,0x08,0xc3,0xec,0xdb,0xa1,0x8d,0x3d,
        0x97,0x00,0xcf,0x2b,0x76,0x82,0xd6,0x1b,0xb5,0xaf,0x6a,0x50,0x45,0xf3,0x30,0xef,
        0x3f,0x55,0xa2,0xea,0x65,0xba,0x2f,0xc0,0xde,0x1c,0xfd,0x4d,0x92,0x75,0x06,0x8a,
        0xb2,0xe6,0x0e,0x1f,0x62,0xd4,0xa8,0x96,0xf9,0xc5,0x25,0x59,0x84,0x72,0x39,0x4c,
        0x5e,0x78,0x38,0x8c,0xd1,0xa5,0xe2,0x61,0xb3,0x21,0x9c,0x1e,0x43,0xc7,0xfc,0x04,
        0x51,0x99,0x6d,0x0d,0xfa,0xdf,0x7e,0x24,0x3b,0xab,0xce,0x11,0x8f,0x4e,0xb7,0xeb,
        0x3c,0x81,0x94,0xf7,0xb9,0x13,0x2c,0xd3,0xe7,0x6e,0xc4,0x03,0x56,0x44,0x7f,0xa9,
        0x2a,0xbb,0xc1,0x53,0xdc,0x0b,0x9d,0x6c,0x31,0x74,0xf6,0x46,0xac,0x89,0x14,0xe1,
        0x16,0x3a,0x69,0x09,0x70,0xb6,0xd0,0xed,0xcc,0x42,0x98,0xa4,0x28,0x5c,0xf8,0x86
    };
    static const unsigned char C[64] = {
        1,1,4,1,8,5,2,9,
        9,1,1,4,1,8,5,2,
        2,9,1,1,4,1,8,5,
        5,2,9,1,1,4,1,8,
        8,5,2,9,1,1,4,1,
        1,8,5,2,9,1,1,4,
        4,1,8,5,2,9,1,1,
        1,4,1,8,5,2,9,1
    };

    size_t new_len = len + 1;
    while (new_len % 64 != 32)
        new_len++;
    new_len += 32;
    unsigned char *msg = malloc(new_len);
    if (!msg)
        return NULL;
    memcpy(msg, input, len);
    msg[len] = 0x80;
    for (size_t i = len + 1; i < new_len; i++)
        msg[i] = 0;
    uint64_t bit_len = (uint64_t)len * 8;
    for (int i = 0; i < 8; i++)
        msg[new_len - 1 - i] = (bit_len >> (8 * i)) & 0xff;

    unsigned char H[64] = {0};

    for (size_t offset = 0; offset < new_len; offset += 64)
    {
        unsigned char M[64];
        unsigned char K[64];
        unsigned char state[64];
        unsigned char tmp[64];
        unsigned char out[64];

        memcpy(M, msg + offset, 64);
        memcpy(K, H, 64);

        for (int i = 0; i < 64; i++)
            state[i] = M[i] ^ K[i];
        for (int round = 1; round <= 10; round++) {
            for (int i = 0; i < 64; i++)
                tmp[i] = S[K[i]];
            for (int row = 0; row < 8; row++)
                for (int col = 0; col < 8; col++)
                    out[row * 8 + col] = tmp[((row - col + 8) % 8) * 8 + col];
            for (int row = 0; row < 8; row++) {
                for (int col = 0; col < 8; col++) {
                    unsigned char acc = 0;
                    for (int k = 0; k < 8; k++) {
                        unsigned char a = out[row * 8 + k];
                        unsigned char b = C[k * 8 + col];
                        unsigned char p = 0;
                        for (int bit = 0; bit < 8; bit++) {
                            if (b & 1)
                                p ^= a;
                            if (a & 0x80)
                                a = (a << 1) ^ 0x1d;
                            else
                                a <<= 1;
                            b >>= 1;
                        }
                        acc ^= p;
                    }
                    tmp[row * 8 + col] = acc;
                }
            }
            for (int i = 0; i < 64; i++)
                K[i] = tmp[i];
            for (int i = 0; i < 8; i++)
                K[i] ^= S[8 * (round - 1) + i];
            for (int i = 0; i < 64; i++)
                tmp[i] = S[state[i]];
            for (int row = 0; row < 8; row++)
                for (int col = 0; col < 8; col++)
                    out[row * 8 + col] = tmp[((row - col + 8) % 8) * 8 + col];
            for (int row = 0; row < 8; row++) {
                for (int col = 0; col < 8; col++) {
                    unsigned char acc = 0;

                    for (int k = 0; k < 8; k++) {
                        unsigned char a = out[row * 8 + k];
                        unsigned char b = C[k * 8 + col];
                        unsigned char p = 0;

                        for (int bit = 0; bit < 8; bit++)
                        {
                            if (b & 1)
                                p ^= a;
                            if (a & 0x80)
                                a = (a << 1) ^ 0x1d;
                            else
                                a <<= 1;
                            b >>= 1;
                        }
                        acc ^= p;
                    }
                    tmp[row * 8 + col] = acc;
                }
            }
            for (int i = 0; i < 64; i++)
                state[i] = tmp[i] ^ K[i];
        }
        for (int i = 0; i < 64; i++)
            H[i] ^= state[i] ^ M[i];
    }

    char *hash = malloc(129);
    if (!hash) {
        free(msg);
        return NULL;
    }
    static const char *hex = "0123456789abcdef";
    for (int i = 0; i < 64; i++) {
        hash[i * 2] = hex[H[i] >> 4];
        hash[i * 2 + 1] = hex[H[i] & 0x0f];
    }
    hash[128] = '\0';
    free(msg);
    return hash;
}

static void print_whirlpool_result(t_whirlpool_cmd* whirlpool, char* hash, t_input_type type, char* source)
{
    if (type == INPUT_STDIN) {
        if (whirlpool->flags.q == false)
            write(STDOUT_FILENO, "whirlpool (stdin) = ", 20);
        write(STDOUT_FILENO, hash, ft_strlen(hash));
        write(STDOUT_FILENO, "\n", 1);
        return;
    }
    if (whirlpool->flags.r == true) {
        write(STDOUT_FILENO, hash, ft_strlen(hash));
        if (whirlpool->flags.q == false) {
            write(STDOUT_FILENO, " ", 1);
            write(STDOUT_FILENO, source, ft_strlen(source));
        }
        write(STDOUT_FILENO, "\n", 1);
        return;
    }
    else {
        if (whirlpool->flags.q == false) {
            write(STDOUT_FILENO, "whirlpool (", 11);
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

static int process_whirlpool_stdin(t_whirlpool_cmd* whirlpool)
{
    size_t len = 0;
    char *input = read_fd(&len, STDIN_FILENO);
    if (!input)
        return 1;
    if (whirlpool->flags.p)
        if (write(STDOUT_FILENO, input, len) < 0)
            return perror("ft_ssl: whirlpool"), 1;
    char* hash = whirlpool_hash(input, len);
    print_whirlpool_result(whirlpool, hash, INPUT_STDIN, input);
    free(hash);
    free(input);
    return 0;
}

static int process_whirlpool_string(t_whirlpool_cmd* whirlpool, char* input)
{
    char* hash = whirlpool_hash(input, ft_strlen(input));
    print_whirlpool_result(whirlpool, hash, INPUT_STR, input);
    free(hash);
    return 0;
}

static int process_whirlpool_file(t_whirlpool_cmd* whirlpool, char* filename)
{
    size_t len = 0;
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return perror(filename), 1;
    char* input = read_fd(&len, fd);
    close(fd);
    char* hash = whirlpool_hash(input, len);
    print_whirlpool_result(whirlpool, hash, INPUT_FILE, filename);
    free(hash);
    free(input);
    return 0;
}

static int process_whirlpool(t_whirlpool_cmd* whirlpool)
{
    int status = 0;
    if (whirlpool->flags.p || !whirlpool->input_count)
        status = process_whirlpool_stdin(whirlpool);
    size_t i = 0;
    while (i < whirlpool->input_count) {
        if (whirlpool->input[i].type == INPUT_STR)
            status = process_whirlpool_string(whirlpool, whirlpool->input[i].value);
        if (whirlpool->input[i].type == INPUT_FILE)
            status = process_whirlpool_file(whirlpool, whirlpool->input[i].value);
        i++;
    }
    return status;
}

int cmd_whirlpool(t_args* args)
{
    t_whirlpool_cmd whirlpool;
    initialize_whirlpool(&whirlpool);
    if (parse_whirlpool_args(args, &whirlpool))
        return 1;
    int status = process_whirlpool(&whirlpool);
    if (whirlpool.input)
        free(whirlpool.input);
    return status;
}