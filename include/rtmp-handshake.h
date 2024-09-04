#ifndef __RTMP_HANDSHAKE_H__
#define __RTMP_HANDSHAKE_H__
#include "net-bitstream.h"
#include <stdint.h>

enum
{
	RTMP_VERSION		= 3,
	RTMP_HANDSHAKE_SIZE	= 1536,
};

typedef enum
{
	RTMP_HANDSHAKE_UNINIT = 0, // Uninitialized 
	RTMP_HANDSHAKE_0, // received C0/S0, wait C1/S1
	RTMP_HANDSHAKE_1, // received C1/S1, wait C2/S2
	RTMP_HANDSHAKE_2, // received C2/S2, handshake done
} rtmp_handshake;

typedef struct 
{
    uint32_t time;
    uint32_t verson;
    uint8_t key[764];
    uint8_t digest[764];
} handshakeSchema0C1S1;

typedef struct 
{
    uint32_t time;
    uint32_t verson;
    uint8_t digest[764];
    uint8_t key[764];
} handshakeSchema1C1S1;

typedef struct 
{
    uint8_t random_data[1504];
    uint8_t digest_data[32];
} handshakeC2S2;

int rtmp_build_handshake_C0(bs_t *b, int version);
int rtmp_build_handshake_C1(bs_t *b, uint32_t timestamp);
int rtmp_build_handshake_C2(bs_t *b, uint32_t timestamp, uint8_t *s1, size_t bytes);

int rtmp_build_handshake_S0(bs_t *b, int version);
int rtmp_build_handshake_S1(bs_t *b, uint32_t timestamp, uint8_t *c1, size_t bytes);
int rtmp_build_handshake_S2(bs_t *b, uint32_t timestamp, uint8_t *c1, size_t bytes);

#endif // !__RTMP_HANDSHAKE_H__
