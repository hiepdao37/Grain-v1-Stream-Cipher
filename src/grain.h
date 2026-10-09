
#ifndef GRAIN_H
#define GRAIN_H

#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t nfsr[80];
    uint8_t lfsr[80];
} GrainState;

void grain_init(GrainState *g,
                const uint8_t key[10],
                const uint8_t iv[8]);

uint8_t grain_next_bit(GrainState *g);

void grain_keystream(GrainState *g,
                     uint8_t *out,
                     size_t bit_count);

void grain_xor(const uint8_t *input,
               uint8_t *output,
               const uint8_t *keystream,
               size_t length);

#endif
