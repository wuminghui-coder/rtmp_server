#include "rtmp-chunk-header.h"
#include "log.h"
#include "net-bitstream.h"
#include "rtmp-session.h"
/*
    表示chunk type，取值[0, 3]，即chunk共有4种类型
    csid范围是3~65599，0~2为协议保留用作特殊信息；
    通常控制流csid为2，命令流为3，开发中发现音视频流csid可自定义，如音频流4，视频流6
*/

// 5.3.1.1. Chunk Basic Header (p12)
/*
 0 1 2 3 4 5 6 7
+-+-+-+-+-+-+-+-+
|fmt|   cs id   |
+-+-+-+-+-+-+-+-+

 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|fmt|     0     |   cs id - 64  |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|fmt|     1     |          cs id - 64           |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
// 5.3.1.2. Chunk Message Header (p13)
/*
 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|                   timestamp                   |message length |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
| message length (cont)         |message type id| msg stream id |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
|           message stream id (cont)            |
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
int rtmp_chunk_write_basic_header(bs_t *b, rtmp_chunk_header *header)
{
    if (b == NULL || header == NULL || bs_bytes_left(b) < 1)
        return -1;

    bs_write_u(b, 2, header->fmt);
    if (header->csid >= 64 + 255)
    {
        bs_write_u(b, 6 , 1);
        bs_write_u(b, 16, header->csid);
    }
    else if (header->csid >= 64)
    {
        bs_write_u(b, 6, 0);
        bs_write_u(b, 8, header->csid);
    } else {
        bs_write_u(b, 6, header->csid);
    }
    return 0;
}

int rtmp_chunk_write_message_header(bs_t *b, rtmp_chunk_header *header)
{
    if (b == NULL)
        return -1;

    if (header->fmt == RTMP_CHUNK_TYPE_0)
    {
        bs_write_u(b, 24, header->timestamp >= 0xFFFFFF ? 0xFFFFFF : header->timestamp);
        bs_write_u(b, 24, header->length);
        bs_write_u(b, 8, header->type_id);
        bs_write_u(b, 32, header->stream_id);
    }
    else if (header->fmt == RTMP_CHUNK_TYPE_1)
    {
        bs_write_u(b, 24, header->timestamp >= 0xFFFFFF ? 0xFFFFFF : header->timestamp);
        bs_write_u(b, 24, header->length);
        bs_write_u(b, 8, header->type_id);
        bs_write_u(b, 32, header->stream_id);
    }
    else if (header->fmt == RTMP_CHUNK_TYPE_2)
    {
        bs_write_u(b, 24, header->timestamp >= 0xFFFFFF ? 0xFFFFFF : header->timestamp);
    }
    return 0;
}

int rtmp_chunk_write_extended_timestamp(bs_t *b, uint32_t timestamp)
{
    if (bs_bytes_left(b) < 4)
    {
        return -1;
    }

    bs_write_u(b, 32, timestamp);
    return 0;
}

int rtmp_chunk_write_header(bs_t *b, uint8_t fmt, uint8_t csid, uint8_t message_id, uint8_t stream_id, uint32_t message_length, uint32_t timestamp)
{
    if (fmt == RTMP_CHUNK_TYPE_0 && bs_bytes_left(b) < 11)
        return NET_FAIL;
    if (fmt == RTMP_CHUNK_TYPE_1 && bs_bytes_left(b) < 7)
        return NET_FAIL;
    if (fmt == RTMP_CHUNK_TYPE_2 && bs_bytes_left(b) < 3)
        return NET_FAIL;
 
    rtmp_chunk_header header = {
        .fmt = fmt,
        .csid = csid,
        .timestamp = timestamp,
        .length = message_length,
        .type_id = message_id,
        .stream_id = stream_id,
    };

    rtmp_chunk_write_basic_header(b, &header);
    rtmp_chunk_write_message_header(b, &header);

    return NET_SUCCESS;
}

int rtmp_chunk_write_header_type(bs_t *b, rtmp_chunk_header *header)
{
    if (b == NULL || header == NULL)
        return NET_FAIL;

    rtmp_chunk_write_basic_header(b, header);
    rtmp_chunk_write_message_header(b, header);

    return NET_SUCCESS;
}

int rtmp_chunk_read_basic_header(bs_t *b, rtmp_chunk_header *header)
{
    if (!b || bs_bytes_left(b) < 1 || !header)
    {
        ERR("args is error");
        return NET_FAIL;
    }

    header->fmt = bs_read_u(b, 2);
    header->csid = bs_read_u(b, 6);

    if (header->csid == 0)
    {
        header->csid = bs_read_u8(b) + 64;
    }
    else if (header->csid == 1)
    {
        header->csid = bs_read_u(b, 16) + 64 + 255;
    }

    return NET_SUCCESS;
}

int rtmp_chunk_read_message_header(bs_t *b, rtmp_chunk_header *header)
{
    if (!b|| !header)
    {
        ERR("args is error");
        return NET_FAIL;
    }

    if (header->fmt == RTMP_CHUNK_TYPE_3)
        return NET_SUCCESS;

    if (header->fmt == RTMP_CHUNK_TYPE_0)
    {
        header->timestamp = bs_read_u(b, 24);
        header->length    = bs_read_u(b, 24);
        header->type_id   = bs_read_u(b, 8);
        header->stream_id = bs_read_u(b, 32);
        if (header->timestamp >= 0xffffff)
        {
            header->timestamp = bs_read_u(b, 32);
        }
    }
    else if (header->fmt == RTMP_CHUNK_TYPE_1)
    {
        header->timestamp = bs_read_u(b, 24);
        header->length    = bs_read_u(b, 24);
        header->type_id   = bs_read_u(b, 8);
        if (header->timestamp >= 0xffffff)
        {
            header->timestamp = bs_read_u(b, 32);
        }
    }
    else if (header->fmt == RTMP_CHUNK_TYPE_2)
    {
        header->timestamp  = bs_read_u(b, 24);
        if (header->timestamp >= 0xffffff)
        {
            header->timestamp = bs_read_u(b, 32);
        }
    }

    return NET_SUCCESS;
}

int rtmp_chunk_show(rtmp_chunk_header *header)
{
    if (header == NULL)
        return NET_FAIL;

    LOG("fmt %d, csid %d, timestemp %d, length %d, type %d, stream %d", header->fmt, header->csid, header->timestamp, header->length, header->type_id, header->stream_id);
}
