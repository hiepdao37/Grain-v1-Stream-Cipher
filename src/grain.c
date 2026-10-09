
#include "grain.h"
#include <string.h>

/*
 * Grain v1 Stream Cipher
 * Key: 80 bits (10 bytes)
 * IV : 64 bits (8 bytes)
 * LFSR and NFSR: 80 bits each
 */

/* Output function h(x) */
static uint8_t output_h(const GrainState *g)
{
    const uint8_t *s = g->lfsr;
    const uint8_t *b = g->nfsr;

    uint8_t x0 = s[3];
    uint8_t x1 = s[25];
    uint8_t x2 = s[46];
    uint8_t x3 = s[64];
    uint8_t x4 = b[63];

    return x0 ^ x1 ^ x2 ^ x3 ^ x4
        ^ (x1 & x4)
        ^ (x0 & x3)
        ^ (x2 & x3)
        ^ (x3 & x4)
        ^ (x0 & x1 & x2)
        ^ (x0 & x2 & x3)
        ^ (x0 & x2 & x4)
        ^ (x1 & x2 & x4)
        ^ (x2 & x3 & x4);
}

/* Feedback function of LFSR */
static uint8_t lfsr_feedback(const GrainState *g)
{
    const uint8_t *s = g->lfsr;

    return s[0] ^ s[13] ^ s[23] ^ s[38]
         ^ s[51] ^ s[62];
}

/* Feedback function of NFSR */
static uint8_t nfsr_feedback(const GrainState *g)
{
    const uint8_t *b = g->nfsr;

    return b[0] ^ b[9] ^ b[14] ^ b[21]
         ^ b[28] ^ b[33] ^ b[37] ^ b[45]
         ^ b[52] ^ b[60] ^ b[62] ^ b[63]
         ^ (b[63] & b[60])
         ^ (b[37] & b[33])
         ^ (b[15] & b[9])
         ^ (b[60] & b[52] & b[45])
         ^ (b[33] & b[28] & b[21])
         ^ (b[63] & b[45] & b[28] & b[9])
         ^ (b[60] & b[52] & b[37] & b[33])
         ^ (b[63] & b[60] & b[21] & b[15])
         ^ (b[63] & b[60] & b[52] & b[45] & b[37])
         ^ (b[33] & b[28] & b[21] & b[15] & b[9])
         ^ (b[52] & b[45] & b[37] & b[33] & b[28] & b[21]);
}

/* Shift register and append a new bit */
static void shift_register(uint8_t reg[80], uint8_t new_bit)
{
    memmove(&reg[0], &reg[1], 79 * sizeof(uint8_t));
    reg[79] = new_bit & 1;
}

/* Initialize Grain v1 */
void grain_init(GrainState *g,
                const uint8_t key[10],
                const uint8_t iv[8])
{
    for (int i = 0; i < 80; i++) {
        g->nfsr[i] = (key[i / 8] >> (i % 8)) & 1;
    }

    for (int i = 0; i < 64; i++) {
        g->lfsr[i] = (iv[i / 8] >> (i % 8)) & 1;
    }

    for (int i = 64; i < 80; i++) {
        g->lfsr[i] = 1;
    }

    /* 160 initialization clocks */
    for (int i = 0; i < 160; i++) {
        uint8_t z = output_h(g)
                  ^ g->nfsr[0]
                  ^ g->lfsr[0];

        uint8_t new_n = nfsr_feedback(g) ^ g->lfsr[0] ^ z;
        uint8_t new_s = lfsr_feedback(g) ^ z;

        shift_register(g->nfsr, new_n);
        shift_register(g->lfsr, new_s);
    }
}

/* Generate one keystream bit */
uint8_t grain_next_bit(GrainState *g)
{
    uint8_t z = output_h(g);

    uint8_t new_n = nfsr_feedback(g) ^ g->lfsr[0];
    uint8_t new_s = lfsr_feedback(g);

    shift_register(g->nfsr, new_n);
    shift_register(g->lfsr, new_s);

    return z & 1;
}

/* Generate bit_count keystream bits, packed MSB first */
void grain_keystream(GrainState *g,
                     uint8_t *out,
                     size_t bit_count)
{
    size_t byte_count = (bit_count + 7) / 8;

    memset(out, 0, byte_count);

    for (size_t i = 0; i < bit_count; i++) {
        uint8_t bit = grain_next_bit(g);
        out[i / 8] |= bit << (7 - (i % 8));
    }
}

/* XOR input with keystream to encrypt/decrypt */
void grain_xor(const uint8_t *input,
               uint8_t *output,
               const uint8_t *keystream,
               size_t length)
{
    for (size_t i = 0; i < length; i++) {
        output[i] = input[i] ^ keystream[i];
    }
}
