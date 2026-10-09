
#include <stdio.h>
#include <stdint.h>
#include "grain.h"

int main(void)
{
    uint8_t key[10] = {0};
    uint8_t iv[8] = {0};
    uint8_t keystream[10];

    GrainState g;

    grain_init(&g, key, iv);
    grain_keystream(&g, keystream, 80);

    printf("====================================\n");
    printf("       GRAIN v1 STREAM CIPHER\n");
    printf("====================================\n");

    printf("Key       : ");
    for (int i = 0; i < 10; i++) {
        printf("%02x", key[i]);
    }

    printf("\nIV        : ");
    for (int i = 0; i < 8; i++) {
        printf("%02x", iv[i]);
    }

    printf("\nKeystream : ");
    for (int i = 0; i < 10; i++) {
        printf("%02x", keystream[i]);
    }

    printf("\n====================================\n");

    return 0;
}

