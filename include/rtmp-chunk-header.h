#ifndef __RTMP_CHUNK_HEADER_H__
#define __RTMP_CHUNK_HEADER_H__
#include <stdint.h>
#include "net-bitstream.h"

enum rtmp_chunk_type_t
{
	RTMP_CHUNK_TYPE_0 = 0, // 11-bytes: timestamp(3) + length(3) + stream type(1) + stream id(4)
	RTMP_CHUNK_TYPE_1 = 1, // 7-bytes: delta(3) + length(3) + stream type(1)
	RTMP_CHUNK_TYPE_2 = 2, // 3-bytes: delta(3)
	RTMP_CHUNK_TYPE_3 = 3, // 0-byte
};

enum rtmp_channel_t
{
	RTMP_CHANNEL_PROTOCOL = 2,	// Protocol Control Messages (1,2,3,5,6) & User Control Messages Event (4)
	RTMP_CHANNEL_INVOKE,		// RTMP_TYPE_INVOKE (20) & RTMP_TYPE_FLEX_MESSAGE (17)
	RTMP_CHANNEL_AUDIO,			// RTMP_TYPE_AUDIO (8)
	RTMP_CHANNEL_VIDEO,			// RTMP_TYPE_VIDEO (9)
	RTMP_CHANNEL_DATA,			// RTMP_TYPE_DATA (18) & RTMP_TYPE_FLEX_STREAM (15)
    RTMP_CHANNEL_DATA1,			// RTMP_TYPE_DATA (18) & RTMP_TYPE_FLEX_STREAM (15)
	RTMP_CHANNEL_MAX = 65599,	// The protocol supports up to 65597 streams with IDs 3-65599(65535 + 64)
};

typedef struct
{
    uint8_t  fmt;       // RTMP_CHUNK_TYPE_XXX
    uint8_t  csid;      // chunk stream id(22-bits)
    uint32_t timestamp; // delta(24-bits) / extended timestamp(32-bits)
    uint32_t length;    // message length (24-bits)
    uint8_t  type_id;   // message type id
    uint32_t stream_id; // message stream id
} rtmp_chunk_header;

int rtmp_chunk_write_basic_header(bs_t *b, rtmp_chunk_header *header);
int rtmp_chunk_write_message_header(bs_t *b, rtmp_chunk_header *header);
int rtmp_chunk_write_extended_timestamp(bs_t *b, uint32_t timestamp);
int rtmp_chunk_write_header(bs_t *b, uint8_t fmt, uint8_t csid, uint8_t message_id, uint8_t stream_id, uint32_t message_length, uint32_t timestamp);
int rtmp_chunk_write_header_type(bs_t *b, rtmp_chunk_header *header);

int rtmp_chunk_read_basic_header(bs_t *b, rtmp_chunk_header *header);
int rtmp_chunk_read_message_header(bs_t *b, rtmp_chunk_header *header);

int rtmp_chunk_show(rtmp_chunk_header *header);

static inline int get_chunk_length(int type)
{
    if (RTMP_CHUNK_TYPE_0 == type)
        return 11;
    else if (RTMP_CHUNK_TYPE_1 == type)
        return 7;
    else if (RTMP_CHUNK_TYPE_2 == type)
        return 3;
    return 0;
}

#endif // !__RTMP_CHUNK_HEADER_H__
