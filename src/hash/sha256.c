#include "hash/sha256.h"
#include "utils/utils.h"
#include "io/input.h"

static void initialize_sha256(t_sha256_cmd* sha256)
{
    sha256->flags.p = false;
    sha256->flags.q = false;
    sha256->flags.r = false;
    sha256->input_count = 0;
    sha256->input = NULL;
}

static int validate_sha256_flags(char* flag, t_flags* flags)
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

static int parse_sha256_args(t_args* args, t_sha256_cmd* sha256)
{
    sha256->input_capacity = args->argc;
    if (sha256->input_capacity > 0) {
        sha256->input = malloc(sizeof(t_hash_input) * sha256->input_capacity);
        if (!sha256->input)
            return perror("ft_ssl: sha256"), 1;
    }
    
    int i = 0;
    while(args->argv && args->argv[i] && args->argv[i][0] == '-') {
        if (!ft_strcmp(args->argv[i], "-s")) {
            if (!args->argv[i + 1])
                return print_usage(), free(sha256->input), 1;
            sha256->input_count++;
            sha256->input[sha256->input_count - 1].type = INPUT_STR;
            sha256->input[sha256->input_count - 1].value = args->argv[i + 1];
            i+= 2;
            continue;
        }
        if (validate_sha256_flags(&args->argv[i][1], &sha256->flags))
            return free(sha256->input), 1;
        i++;
    }
    while(args->argv && args->argv[i]) {
        sha256->input_count++;
        sha256->input[sha256->input_count - 1].type = INPUT_FILE;
        sha256->input[sha256->input_count - 1].value = args->argv[i];
        i++;
    }
    return 0;
}

static char* sha256_hash(char *input, size_t len)
{
    uint32_t K[64] = {
        0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,
        0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
        0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
        0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
        0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,
        0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
        0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,
        0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
        0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
        0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
        0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,
        0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
        0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,
        0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
        0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
        0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
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
        msg[new_len - 1 - i] = (bit_len >> (8 * i)) & 0xff;

    uint32_t H[8] = {
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
    };

    for (size_t offset = 0; offset < new_len; offset += 64) {
        uint32_t W[64];
        for (int i = 0; i < 16; i++) {
            W[i] = ((uint32_t)msg[offset + i * 4] << 24)
                 | ((uint32_t)msg[offset + i * 4 + 1] << 16)
                 | ((uint32_t)msg[offset + i * 4 + 2] << 8)
                 | ((uint32_t)msg[offset + i * 4 + 3]);
        }
        for (int i = 16; i < 64; i++) {
            uint32_t s0;
            uint32_t s1;

            s0 = ((W[i - 15] >> 7) | (W[i - 15] << 25))
               ^ ((W[i - 15] >> 18) | (W[i - 15] << 14))
               ^ (W[i - 15] >> 3);

            s1 = ((W[i - 2] >> 17) | (W[i - 2] << 15))
               ^ ((W[i - 2] >> 19) | (W[i - 2] << 13))
               ^ (W[i - 2] >> 10);

            W[i] = W[i - 16] + s0 + W[i - 7] + s1;
        }

        uint32_t a = H[0];
        uint32_t b = H[1];
        uint32_t c = H[2];
        uint32_t d = H[3];
        uint32_t e = H[4];
        uint32_t f = H[5];
        uint32_t g = H[6];
        uint32_t h = H[7];

        for (int i = 0; i < 64; i++) {
            uint32_t S1;
            uint32_t ch;
            uint32_t temp1;
            uint32_t S0;
            uint32_t maj;
            uint32_t temp2;

            S1 = ((e >> 6) | (e << 26))
               ^ ((e >> 11) | (e << 21))
               ^ ((e >> 25) | (e << 7));
            ch = (e & f) ^ ((~e) & g);
            temp1 = h + S1 + ch + K[i] + W[i];
            S0 = ((a >> 2) | (a << 30))
               ^ ((a >> 13) | (a << 19))
               ^ ((a >> 22) | (a << 10));
            maj = (a & b) ^ (a & c) ^ (b & c);
            temp2 = S0 + maj;
            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }
        H[0] += a;
        H[1] += b;
        H[2] += c;
        H[3] += d;
        H[4] += e;
        H[5] += f;
        H[6] += g;
        H[7] += h;
    }

    char *out = malloc(65);
    if (!out) {
        free(msg);
        return NULL;
    }
    char *hex = "0123456789abcdef";
    for (int i = 0; i < 8; i++) {
        out[i * 8 + 0] = hex[(H[i] >> 28) & 0xf];
        out[i * 8 + 1] = hex[(H[i] >> 24) & 0xf];
        out[i * 8 + 2] = hex[(H[i] >> 20) & 0xf];
        out[i * 8 + 3] = hex[(H[i] >> 16) & 0xf];
        out[i * 8 + 4] = hex[(H[i] >> 12) & 0xf];
        out[i * 8 + 5] = hex[(H[i] >> 8) & 0xf];
        out[i * 8 + 6] = hex[(H[i] >> 4) & 0xf];
        out[i * 8 + 7] = hex[H[i] & 0xf];
    }
    out[64] = '\0';
    free(msg);
    return out;
}

static void print_sha256_result(t_sha256_cmd* sha256, char* hash, t_input_type type, char* source)
{
    if (type == INPUT_STDIN) {
        if (sha256->flags.q == false)
            write(STDOUT_FILENO, "sha256 (stdin) = ", 17);
        write(STDOUT_FILENO, hash, ft_strlen(hash));
        write(STDOUT_FILENO, "\n", 1);
        return;
    }
    if (sha256->flags.r == true) {
        write(STDOUT_FILENO, hash, ft_strlen(hash));
        if (sha256->flags.q == false) {
            write(STDOUT_FILENO, " ", 1);
            write(STDOUT_FILENO, source, ft_strlen(source));
        }
        write(STDOUT_FILENO, "\n", 1);
        return;
    }
    else {
        if (sha256->flags.q == false) {
            write(STDOUT_FILENO, "sha256 (", 8);
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

static int process_sha256_stdin(t_sha256_cmd* sha256)
{
    size_t len = 0;
    char *input = read_fd(&len, STDIN_FILENO);
    if (!input)
        return 1;
    if (sha256->flags.p)
        if (write(STDOUT_FILENO, input, len) < 0)
            return perror("ft_ssl: sha256"), 1;
    char* hash = sha256_hash(input, len);
    print_sha256_result(sha256, hash, INPUT_STDIN, input);
    free(hash);
    free(input);
    return 0;
}

static int process_sha256_string(t_sha256_cmd* sha256, char* input)
{
    char* hash = sha256_hash(input, ft_strlen(input));
    print_sha256_result(sha256, hash, INPUT_STR, input);
    free(hash);
    return 0;
}

static int process_sha256_file(t_sha256_cmd* sha256, char* filename)
{
    size_t len = 0;
    int fd = open(filename, O_RDONLY);
    if (fd < 0)
        return perror(filename), 1;
    char* input = read_fd(&len, fd);
    close(fd);
    char* hash = sha256_hash(input, len);
    print_sha256_result(sha256, hash, INPUT_FILE, filename);
    free(hash);
    free(input);
    return 0;
}

static int process_sha256(t_sha256_cmd* sha256)
{
    int status = 0;
    if (sha256->flags.p || !sha256->input_count)
        status = process_sha256_stdin(sha256);
    size_t i = 0;
    while (i < sha256->input_count) {
        if (sha256->input[i].type == INPUT_STR)
            status = process_sha256_string(sha256, sha256->input[i].value);
        if (sha256->input[i].type == INPUT_FILE)
            status = process_sha256_file(sha256, sha256->input[i].value);
        i++;
    }
    return status;
}

int cmd_sha256(t_args* args)
{
    t_sha256_cmd sha256;
    initialize_sha256(&sha256);
    if (parse_sha256_args(args, &sha256))
        return 1;
    int status = process_sha256(&sha256);
    if (sha256.input)
        free(sha256.input);
    return status;
}