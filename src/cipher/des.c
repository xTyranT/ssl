#include "cipher/des.h"
#include "utils/utils.h"
#include "io/input.h"
#include "io/output.h"

#define DES_BLOCK_SIZE 8

static void initialize_des(t_des_cmd* des)
{
    des->mode = ENCRYPT;
    des->input_type = DES_STDIN;
    des->in_file = NULL;
    des->out_file = NULL;
    des->base64 = false;
    des->key = NULL;
    des->key_generated = false;
    des->password = NULL;
    des->p_flag = false;
    des->password = NULL;
    des->salt = NULL;
    des->vector = NULL;
    des->mode_type = DES_MODE_ECB;
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
         || !ft_strcmp(args->argv[i], "-k") || !ft_strcmp(args->argv[i], "-s") || !ft_strcmp(args->argv[i], "-v")) {
            if (!args->argv[i + 1])
                return des_error_missing_arg(), 1;
            if (!ft_strcmp(args->argv[i], "-i")) {
                des->in_file = args->argv[i + 1];
                des->input_type = DES_FILE;
            } else if (!ft_strcmp(args->argv[i], "-o")) {
                des->out_file = args->argv[i + 1];
            } else if (!ft_strcmp(args->argv[i], "-k")) {
                des->key = args->argv[i + 1];
            } else if (!ft_strcmp(args->argv[i], "-s")) {
                des->salt = args->argv[i + 1];
            } else if (!ft_strcmp(args->argv[i], "-v")) {
                des->vector = args->argv[i + 1];
            }
            i += 2;
            continue;
        }
        if (!ft_strcmp(args->argv[i], "-p")) {
            if (args->argv[i + 1] && args->argv[i + 1][0] != '-') {
                des->password = args->argv[i + 1];
                i += 2;
            } else {
                des->p_flag = true;
                i += 1;
            }
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
    if (!des->key) {
        int urandom = open("/dev/urandom", O_RDONLY);
        if (urandom < 0)
            return perror("ft_ssl: des: /dev/urandom"), 1;
        unsigned char bytes[8];
        ssize_t read_len = read(urandom, bytes, sizeof(bytes));
        close(urandom);
        if (read_len != sizeof(bytes))
            return perror("ft_ssl: des: read /dev/urandom"), 1;
        char* generated = malloc(17);
        if (!generated)
            return perror("malloc:"), 1;
        const char* hex = "0123456789abcdef";
        for (int i = 0; i < 8; i++) {
            generated[i * 2] = hex[(bytes[i] >> 4) & 0xF];
            generated[i * 2 + 1] = hex[bytes[i] & 0xF];
        }
        generated[16] = '\0';
        des->key = generated;
        des->key_generated = true;
        fprintf(stderr, "ft_ssl: des: generated key %s\n", generated);
        if (des->mode == DECRYPT)
            fprintf(stderr, "ft_ssl: des: warning: generated key used for decryption may not match the original encryption key\n");
    }
    if (des->key) {
        size_t key_len = ft_strlen(des->key);
        if (key_len > 16)
            return fprintf(stderr, "ft_ssl: des: key must be 16 hex characters (64 bits)\n"), 1;
        if (key_len < 16) {
            char* padded = malloc(17);
            if (!padded)
                return perror("malloc:"), 1;
            for (size_t i = 0; i < key_len; i++)
                padded[i] = des->key[i];
            for (size_t i = key_len; i < 16; i++)
                padded[i] = '0';
            padded[16] = '\0';
            des->key = padded;
        }
    }
    if (des->salt && ft_strlen(des->salt) != 16)
        return fprintf(stderr, "ft_ssl: des: salt must be 16 hex characters (64 bits)\n"), 1;
    if (des->vector) {
        if (ft_strlen(des->vector) != 16)
            return fprintf(stderr, "ft_ssl: des: initialization vector must be 16 hex characters (64 bits)\n"), 1;
        if (check_hex_string(des->vector))
            return 1;
        if (des->mode_type == DES_MODE_ECB)
            return fprintf(stderr, "ft_ssl: des: initialization vector is not supported in ECB mode\n"), 1;
    } else {
        if (des->mode_type != DES_MODE_ECB)
            return fprintf(stderr, "ft_ssl: des: initialization vector required for this mode\n"), 1;
    }
    if (check_hex_string(des->key))
        return 1;
    if (check_hex_string(des->salt))
        return 1;
    return 0;
}

static const uint8_t g_ip[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7
};

static const uint8_t g_fp[64] = {
    40, 8, 48, 16, 56, 24, 64, 32,
    39, 7, 47, 15, 55, 23, 63, 31,
    38, 6, 46, 14, 54, 22, 62, 30,
    37, 5, 45, 13, 53, 21, 61, 29,
    36, 4, 44, 12, 52, 20, 60, 28,
    35, 3, 43, 11, 51, 19, 59, 27,
    34, 2, 42, 10, 50, 18, 58, 26,
    33, 1, 41, 9, 49, 17, 57, 25
};

static const uint8_t g_expansion[48] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1
};

static const uint8_t g_p_perm[32] = {
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25
};

static const uint8_t g_pc1[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4
};

static const uint8_t g_pc2[48] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32
};

static const uint8_t g_shifts[16] = {
    1, 1, 2, 2,
    2, 2, 2, 2,
    1, 2, 2, 2,
    2, 2, 2, 1
};

static const uint8_t g_sboxes[8][64] = {
    {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
     0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
     4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
     15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13},
    {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
     3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
     0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
     13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9},
    {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
     13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
     13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
     1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12},
    {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
     13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
     10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
     3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14},
    {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
     14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
     4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
     11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3},
    {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
     10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
     9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
     4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13},
    {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
     13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
     1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
     6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12},
    {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
     1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
     7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
     2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}
};

static uint64_t permute(uint64_t value, const uint8_t* table, size_t table_len, size_t input_width)
{
    uint64_t out = 0;
    for (size_t i = 0; i < table_len; i++) {
        uint64_t bit = (value >> (input_width - table[i])) & 1ULL;
        out |= bit << (table_len - 1 - i);
    }
    return out;
}

static uint32_t feistel(uint32_t half, uint64_t subkey)
{
    uint64_t expanded = permute(half, g_expansion, 48, 32);
    uint64_t x = expanded ^ subkey;
    uint32_t result = 0;

    for (int box = 0; box < 8; box++) {
        uint8_t six = (x >> (42 - box * 6)) & 0x3F;
        uint8_t row = ((six & 0x20) >> 4) | (six & 0x01);
        uint8_t col = (six >> 1) & 0x0F;
        uint8_t svalue = g_sboxes[box][row * 16 + col];
        result = (result << 4) | svalue;
    }
    return permute(result, g_p_perm, 32, 32);
}

static void generate_subkeys(uint64_t key, uint64_t subkeys[16])
{
    uint64_t permuted = permute(key, g_pc1, 56, 64);
    uint32_t c = (uint32_t)((permuted >> 28) & 0x0FFFFFFF);
    uint32_t d = (uint32_t)(permuted & 0x0FFFFFFF);

    for (int round = 0; round < 16; round++) {
        c = ((c << g_shifts[round]) | (c >> (28 - g_shifts[round]))) & 0x0FFFFFFF;
        d = ((d << g_shifts[round]) | (d >> (28 - g_shifts[round]))) & 0x0FFFFFFF;
        uint64_t cd = ((uint64_t)c << 28) | d;
        subkeys[round] = permute(cd, g_pc2, 48, 56);
    }
}

static uint64_t bytes_to_u64(const unsigned char* bytes)
{
    uint64_t value = 0;
    for (int i = 0; i < 8; i++)
        value = (value << 8) | bytes[i];
    return value;
}

static void u64_to_bytes(uint64_t value, unsigned char* bytes)
{
    for (int i = 7; i >= 0; i--) {
        bytes[i] = value & 0xFF;
        value >>= 8;
    }
}

static uint64_t des_block(uint64_t block, const uint64_t subkeys[16], bool decrypt)
{
    block = permute(block, g_ip, 64, 64);
    uint32_t left = (uint32_t)(block >> 32);
    uint32_t right = (uint32_t)(block & 0xFFFFFFFF);

    for (int round = 0; round < 16; round++) {
        uint32_t next = right;
        uint64_t key = decrypt ? subkeys[15 - round] : subkeys[round];
        right = left ^ feistel(right, key);
        left = next;
    }

    uint64_t preoutput = ((uint64_t)right << 32) | left;
    return permute(preoutput, g_fp, 64, 64);
}

static int parse_hex_digit(char digit)
{
    if (digit >= '0' && digit <= '9')
        return digit - '0';
    if (digit >= 'a' && digit <= 'f')
        return digit - 'a' + 10;
    if (digit >= 'A' && digit <= 'F')
        return digit - 'A' + 10;
    return -1;
}

static int parse_hex_key(const char* hex, uint64_t* result)
{
    uint64_t key = 0;
    for (int i = 0; i < 16; i++) {
        int value = parse_hex_digit(hex[i]);
        if (value < 0)
            return 1;
        key = (key << 4) | (uint64_t)value;
    }
    *result = key;
    return 0;
}

static size_t pkcs7_pad(unsigned char* buffer, size_t len)
{
    size_t pad_len = DES_BLOCK_SIZE - (len % DES_BLOCK_SIZE);
    if (pad_len == 0)
        pad_len = DES_BLOCK_SIZE;
    for (size_t i = 0; i < pad_len; i++)
        buffer[len + i] = (unsigned char)pad_len;
    return len + pad_len;
}

static size_t pkcs7_unpad(unsigned char* buffer, size_t len)
{
    if (len == 0 || len % DES_BLOCK_SIZE != 0)
        return 0;
    unsigned char pad = buffer[len - 1];
    if (pad == 0 || pad > DES_BLOCK_SIZE)
        return 0;
    for (size_t i = 0; i < pad; i++) {
        if (buffer[len - 1 - i] != pad)
            return 0;
    }
    return len - pad;
}

static char* base64_encode_data(const unsigned char* input, size_t len, size_t* out_len)
{
    static const char table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t output_len = ((len + 2) / 3) * 4;
    char* output = malloc(output_len + 1);
    if (!output)
        return NULL;

    size_t j = 0;
    for (size_t i = 0; i < len; i += 3) {
        unsigned int octet_a = input[i];
        unsigned int octet_b = (i + 1 < len) ? input[i + 1] : 0;
        unsigned int octet_c = (i + 2 < len) ? input[i + 2] : 0;
        unsigned int triple = (octet_a << 16) | (octet_b << 8) | octet_c;
        output[j++] = table[(triple >> 18) & 0x3F];
        output[j++] = table[(triple >> 12) & 0x3F];
        if (i + 1 < len)
            output[j++] = table[(triple >> 6) & 0x3F];
        else
            output[j++] = '=';
        if (i + 2 < len)
            output[j++] = table[triple & 0x3F];
        else
            output[j++] = '=';
    }
    output[j] = '\0';
    *out_len = j;
    return output;
}

static char* base64_decode_data(const char* input, size_t len, size_t* out_len)
{
    if (len % 4 != 0)
        return NULL;
    size_t padding = 0;
    if (len >= 2 && input[len - 1] == '=') padding++;
    if (len >= 2 && input[len - 2] == '=') padding++;
    *out_len = (len / 4) * 3 - padding;
    unsigned char* output = malloc(*out_len);
    if (!output)
        return NULL;
    for (size_t i = 0, j = 0; i < len;) {
        int sextet[4];
        for (int k = 0; k < 4; k++) {
            char c = input[i++];
            if (c >= 'A' && c <= 'Z') sextet[k] = c - 'A';
            else if (c >= 'a' && c <= 'z') sextet[k] = c - 'a' + 26;
            else if (c >= '0' && c <= '9') sextet[k] = c - '0' + 52;
            else if (c == '+') sextet[k] = 62;
            else if (c == '/') sextet[k] = 63;
            else if (c == '=') sextet[k] = 0;
            else { free(output); return NULL; }
        }
        unsigned int triple = (sextet[0] << 18) | (sextet[1] << 12) | (sextet[2] << 6) | sextet[3];
        if (j < *out_len) output[j++] = (triple >> 16) & 0xFF;
        if (j < *out_len) output[j++] = (triple >> 8) & 0xFF;
        if (j < *out_len) output[j++] = triple & 0xFF;
    }
    return (char*)output;
}

static int write_output(t_des_cmd* des, const void* data, size_t len)
{
    int fd = des->out_file ? open(des->out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644) : STDOUT_FILENO;
    if (fd < 0)
        return perror("open:"), 1;
    int status = write_to_fd(fd, len, (char*)data);
    if (des->out_file)
        close(fd);
    return status;
}

static int process_des_buffer(char* buffer, size_t len, t_des_cmd* des)
{
    uint64_t key;
    if (!des->key || parse_hex_key(des->key, &key))
        return fprintf(stderr, "ft_ssl: des: invalid key\n"), 1;
    uint64_t subkeys[16];
    generate_subkeys(key, subkeys);
    size_t output_len = len;
    unsigned char* output;
    unsigned char* data = (unsigned char*)buffer;
    if (des->mode == ENCRYPT) {
        size_t padded_len = ((len + DES_BLOCK_SIZE - 1) / DES_BLOCK_SIZE) * DES_BLOCK_SIZE;
        if (len % DES_BLOCK_SIZE == 0)
            padded_len += DES_BLOCK_SIZE;
        output = malloc(padded_len);
        if (!output)
            return perror("malloc:"), 1;
        memcpy(output, data, len);
        output_len = pkcs7_pad(output, len);

        uint64_t iv = 0;
        uint64_t counter = 0;
        if (des->mode_type != DES_MODE_ECB) {
            if (parse_hex_key(des->vector, &iv))
                return fprintf(stderr, "ft_ssl: des: invalid initialization vector\n"), 1;
            counter = iv;
        }

        for (size_t index = 0; index < output_len; index += DES_BLOCK_SIZE) {
            uint64_t block = bytes_to_u64(output + index);
            uint64_t out_block = 0;
            switch (des->mode_type) {
                case DES_MODE_ECB: {
                    uint64_t encrypted = des_block(block, subkeys, false);
                    out_block = encrypted;
                    break;
                }
                case DES_MODE_CBC: {
                    uint64_t x = block ^ iv;
                    uint64_t encrypted = des_block(x, subkeys, false);
                    out_block = encrypted;
                    iv = encrypted;
                    break;
                }
                case DES_MODE_CFB: {
                    uint64_t ks = des_block(iv, subkeys, false);
                    out_block = block ^ ks;
                    iv = out_block;
                    break;
                }
                case DES_MODE_OFB: {
                    uint64_t ks = des_block(iv, subkeys, false);
                    out_block = block ^ ks;
                    iv = ks;
                    break;
                }
                case DES_MODE_CTR: {
                    uint64_t ks = des_block(counter, subkeys, false);
                    out_block = block ^ ks;
                    counter++;
                    break;
                }
            }
            u64_to_bytes(out_block, output + index);
        }
    } else {
        if (len % DES_BLOCK_SIZE != 0)
            return fprintf(stderr, "ft_ssl: des: encrypted data must be multiple of 8 bytes\n"), 1;
        output = malloc(len);
        if (!output)
            return perror("malloc:"), 1;

        uint64_t iv = 0;
        uint64_t counter = 0;
        if (des->mode_type != DES_MODE_ECB) {
            if (parse_hex_key(des->vector, &iv))
                return fprintf(stderr, "ft_ssl: des: invalid initialization vector\n"), 1;
            counter = iv;
        }

        for (size_t index = 0; index < len; index += DES_BLOCK_SIZE) {
            uint64_t cipher_block = bytes_to_u64(data + index);
            uint64_t out_block = 0;
            switch (des->mode_type) {
                case DES_MODE_ECB: {
                    uint64_t decrypted = des_block(cipher_block, subkeys, true);
                    out_block = decrypted;
                    break;
                }
                case DES_MODE_CBC: {
                    uint64_t decrypted = des_block(cipher_block, subkeys, true);
                    out_block = decrypted ^ iv;
                    iv = cipher_block;
                    break;
                }
                case DES_MODE_CFB: {
                    uint64_t ks = des_block(iv, subkeys, false);
                    out_block = cipher_block ^ ks;
                    iv = cipher_block;
                    break;
                }
                case DES_MODE_OFB: {
                    uint64_t ks = des_block(iv, subkeys, false);
                    out_block = cipher_block ^ ks;
                    iv = ks;
                    break;
                }
                case DES_MODE_CTR: {
                    uint64_t ks = des_block(counter, subkeys, false);
                    out_block = cipher_block ^ ks;
                    counter++;
                    break;
                }
            }
            u64_to_bytes(out_block, output + index);
        }
        if (des->key_generated) {
            output_len = len;
        } else {
            output_len = pkcs7_unpad(output, len);
            if (output_len == 0)
                return free(output), fprintf(stderr, "ft_ssl: des: invalid padding\n"), 1;
        }
    }

    int status;
    if (des->base64 && des->mode == ENCRYPT) {
        size_t encoded_len = 0;
        char* encoded = base64_encode_data(output, output_len, &encoded_len);
        free(output);
        if (!encoded)
            return perror("malloc:"), 1;
        status = write_output(des, encoded, encoded_len);
        free(encoded);
    }
    else {
        status = write_output(des, output, output_len);
        free(output);
    }
    return status;
}

static int process_des_file(t_des_cmd* des)
{
    int fd = open(des->in_file, O_RDONLY);
    if (fd < 0)
        return perror("open:"), 1;
    size_t len = 0;
    char* input = read_fd(&len, fd);
    close(fd);
    if (!input)
        return 1;
    int status;
    if (des->base64 && des->mode == DECRYPT) {
        size_t decoded_len = 0;
        char* decoded = base64_decode_data(input, len, &decoded_len);
        free(input);
        if (!decoded)
            return fprintf(stderr, "ft_ssl: des: invalid base64 input\n"), 1;
        status = process_des_buffer(decoded, decoded_len, des);
        free(decoded);
    } else {
        status = process_des_buffer(input, len, des);
        free(input);
    }
    return status;
}

static int process_des_stdin(t_des_cmd* des)
{
    size_t len = 0;
    char* input = read_fd(&len, STDIN_FILENO);
    if (!input)
        return 1;
    int status;
    if (des->base64 && des->mode == DECRYPT) {
        size_t decoded_len = 0;
        char* decoded = base64_decode_data(input, len, &decoded_len);
        free(input);
        if (!decoded)
            return fprintf(stderr, "ft_ssl: des: invalid base64 input\n"), 1;
        status = process_des_buffer(decoded, decoded_len, des);
        free(decoded);
    } else {
        status = process_des_buffer(input, len, des);
        free(input);
    }
    return status;
}

static int process_des(t_des_cmd* des)
{
    if (des->password)
        return process_des_buffer(des->password, ft_strlen(des->password), des);
    if (des->p_flag) {
        size_t len = 0;
        char* input = read_fd(&len, STDIN_FILENO);
        if (!input)
            return 1;
        if (write(STDOUT_FILENO, input, len) < 0) {
            free(input);
            return perror("ft_ssl: des"), 1;
        }
        int status = process_des_buffer(input, len, des);
        free(input);
        return status;
    }
    if (des->input_type == DES_FILE)
        return process_des_file(des);
    return process_des_stdin(des);
}

int cmd_des(t_args* args)
{
    t_des_cmd des;
    initialize_des(&des);
    if (args && args->command) {
        if (!ft_strcmp(args->command, "des-cbc"))
            des.mode_type = DES_MODE_CBC;
        else if (!ft_strcmp(args->command, "des-cfb"))
            des.mode_type = DES_MODE_CFB;
        else if (!ft_strcmp(args->command, "des-ofb"))
            des.mode_type = DES_MODE_OFB;
        else if (!ft_strcmp(args->command, "des-ctr"))
            des.mode_type = DES_MODE_CTR;
        else
            des.mode_type = DES_MODE_ECB;
    }
    if (parse_des_args(args, &des))
        return 1;
    if (validate_des_args(&des))
        return 1;
    int status = process_des(&des);
    if (des.key_generated)
        free(des.key);
    return status;
}