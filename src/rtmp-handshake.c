#include "rtmp-handshake.h"
#include <assert.h>

int rtmp_handshake_random(bs_t *b, uint32_t timestamp, size_t bytes)
{
    srand(timestamp);
    while (!bs_eof(b) && bytes > 0)
    {
        bs_write_u8(b, rand());
        bytes--;
    }

    return 0;
}

int rtmp_build_handshake_C0(bs_t *b, int version)
{
    if (b == NULL)
        return -1;

    assert(RTMP_VERSION == version);

    bs_write_u8(b, version);

    return 1;
}

int rtmp_build_handshake_C1(bs_t *b, uint32_t timestamp)
{
    if (b == NULL)
        return -1;

    bs_write_u(b, 32, timestamp);
    bs_write_u(b, 32, 0);
    rtmp_handshake_random(b, timestamp, RTMP_HANDSHAKE_SIZE -8);
    return RTMP_HANDSHAKE_SIZE;
}

int rtmp_build_handshake_C2(bs_t *b, uint32_t timestamp, uint8_t *s1, size_t bytes)
{
    assert(RTMP_HANDSHAKE_SIZE == bytes);
    bs_write_u(b, 32, timestamp);
    bs_write_bytes(b, s1, RTMP_HANDSHAKE_SIZE -32);
    return 0;
}

int rtmp_build_handshake_S0(bs_t *b, int version)
{
    if (b == NULL)
        return -1;

    assert(RTMP_VERSION == version);

    bs_write_u8(b,version);

    return 0;
}

int rtmp_build_handshake_S1(bs_t *b, uint32_t timestamp, uint8_t *c1, size_t bytes)
{
    if (b == NULL)
        return -1;

    bs_write_u(b, 32, timestamp);
    bs_write_u(b, 32, 0);
    rtmp_handshake_random(b, timestamp, bytes - 8);
    return RTMP_HANDSHAKE_SIZE;
}

int rtmp_build_handshake_S2(bs_t *b, uint32_t timestamp, uint8_t *c1, size_t bytes)
{
    if (b == NULL)
        return -1;
        
    assert(RTMP_HANDSHAKE_SIZE == bytes);
    bs_write_u(b, 32, timestamp);
    bs_write_bytes(b, c1, RTMP_HANDSHAKE_SIZE - 32);
    return RTMP_HANDSHAKE_SIZE;
}