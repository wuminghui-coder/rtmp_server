#include "rtmp-h264-packet.h"
#include "kernel-list.h"
#include "rtmp-send-message.h"

static int find_nal_unit(uint8_t* buf, int size, int* nal_start, int* nal_end)
{
    int i;
    // find start
    *nal_start = 0;
    *nal_end = 0;
    
    i = 0;
    while (   //( next_bits( 24 ) != 0x000001 && next_bits( 32 ) != 0x00000001 )
        (buf[i] != 0 || buf[i+1] != 0 || buf[i+2] != 0x01) && 
        (buf[i] != 0 || buf[i+1] != 0 || buf[i+2] != 0 || buf[i+3] != 0x01) 
        )
    {
        i++; // skip leading zero
        if (i+4 >= size) { return 0; } // did not find nal start
    }

    if  (buf[i] != 0 || buf[i+1] != 0 || buf[i+2] != 0x01) // ( next_bits( 24 ) != 0x000001 )
    {
        i++;
    }

    if  (buf[i] != 0 || buf[i+1] != 0 || buf[i+2] != 0x01) { /* error, should never happen */ return 0; }
    i+= 3;
    *nal_start = i;
    
    while (   //( next_bits( 24 ) != 0x000000 && next_bits( 24 ) != 0x000001 )
        (buf[i] != 0 || buf[i+1] != 0 || buf[i+2] != 0) && 
        (buf[i] != 0 || buf[i+1] != 0 || buf[i+2] != 0x01) 
        )
    {
        i++;
        // FIXME the next line fails when reading a nal that ends exactly at the end of the data
        if (i+3 >= size) { *nal_end = size; return -1; } // did not find nal end, stream ended first
    }
    
    *nal_end = i;
    return (*nal_end - *nal_start);
}

static int rtmp_write_frame(uint8_t *payload, int payload_size, uint8_t *data, int size, int type)
{
    bs_t *b = bs_new(payload, payload_size);
    if (b == NULL)
        return NET_FAIL;

    if (NAL_UNIT_TYPE_CODED_SLICE_IDR == type)
        bs_write_u8(b, 0x17); //0x27
    else 
        bs_write_u8(b, 0x27); //0x27
                              //
    bs_write_u8(b, 0x01);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);

    bs_write_u(b, 32, size);

    bs_write_bytes(b, data, size);

    int bs_size = bs_pos(b);

    bs_free(b);

    return bs_size; 
}

static int rtmp_write_avc_sequence(h264_stream * stream)
{
    if (stream->stream_info[0] == NULL || stream->stream_info[1] == NULL)
        return NET_FAIL;

    int size = stream->stream_info[0]->size + stream->stream_info[1]->size + 20;

    uint8_t *payload = (uint8_t *)calloc(1, size);
    if (payload == NULL)
        return NET_FAIL;

    bs_t *b = bs_new(payload, size);

    bs_write_u8(b, 0x17);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);

    bs_write_u8(b, 0x01);
    bs_write_u8(b, stream->stream_info[0]->payload[1]);
    bs_write_u8(b, stream->stream_info[0]->payload[2]);
    bs_write_u8(b, stream->stream_info[0]->payload[3]);
    bs_write_u8(b, 0xff);

    bs_write_u8(b, 0xe1);
    bs_write_u(b, 16, stream->stream_info[0]->size);
    bs_write_bytes(b, stream->stream_info[0]->payload, stream->stream_info[0]->size);
    
    bs_write_u8(b, 0x01);
    bs_write_u(b, 16, stream->stream_info[1]->size);
    bs_write_bytes(b, stream->stream_info[1]->payload, stream->stream_info[1]->size);

    shm_cache_put(stream->cache, payload, bs_pos(b), NAL_UNIT_TYPE_SPS);

    bs_free(b);
    net_free(payload);
    return NET_SUCCESS;
}

static int rtmp_paser_packet(h264_stream * stream, uint8_t *data, int size, int type)
{
    if (stream == NULL || data == NULL || size < 0)
        return NET_FAIL;

    if (type == NAL_UNIT_TYPE_SPS || type == NAL_UNIT_TYPE_PPS)
    {
        cache_buffer *cb = (cache_buffer *)calloc(1, sizeof_buffer(size + 20));
        if (cb == NULL)
            return NET_FAIL;

        int channel = type == NAL_UNIT_TYPE_SPS ? 0 : 1;

        if (stream->stream_info[channel])
            net_free(stream->stream_info[channel]);

        stream->stream_info[channel] = cb;
        stream->stream_info[channel]->type = type;
        stream->stream_info[channel]->size = size;
        memcpy(stream->stream_info[channel]->payload, data, size);
    } else {
        uint8_t *payload = (uint8_t *)calloc(1, size + 20);
        int payload_size = rtmp_write_frame(payload, size + 20, data, size, type);
        if (payload_size == NET_FAIL)
        {
            net_free(payload);
            return NET_FAIL;
        }

        if (type == NAL_UNIT_TYPE_CODED_SLICE_IDR)
            rtmp_write_avc_sequence(stream);

        if (stream->cache)
            shm_cache_put(stream->cache, payload, payload_size, type);
        net_free(payload);
    }
    return NET_SUCCESS;
}

int rtmp_push_h264_stream(void *args)
{
    h264_stream * stream  = (h264_stream *)args;
    if (stream == NULL || stream->cache == NULL)
       return NET_FAIL;

    cache_buffer *b = shm_cache_get(stream->cache);
    if (b == NULL)
        return NET_FAIL;
        
    if (stream->rtmp_server)
        tcp_push_stream_connection(stream->rtmp_server, b->payload, b->size, b->type);

    net_free(b);
    return NET_SUCCESS;
}

static int rtmp_pull_h264_stream(void *args)
{
    h264_stream * stream  = (h264_stream *)args;
    if (stream == NULL || stream->fp <= 0 || stream->buffer == NULL)
        return NET_FAIL;

    if (feof(stream->fp)) 
    {
        fseek(stream->fp, 0, SEEK_SET);
        retrieve_all(stream->buffer);
        return NET_SUCCESS;
    }
    
    buffer_fread(stream->buffer, stream->fp);

    int nal_start = 0, nal_end = 0;
    while (readable_bytes(stream->buffer) > 0 
        && find_nal_unit(begin_read(stream->buffer), readable_bytes(stream->buffer), &nal_start, &nal_end) > 0)
    {
        uint8_t *nalu_start = begin_read(stream->buffer) + nal_start;
        rtmp_paser_packet(stream, nalu_start, nal_end - nal_start, (*nalu_start) & 0x1F);
        retrieve(stream->buffer, nal_end);
    }
    return NET_SUCCESS;
}

void h264_stream_unint(h264_stream *stream)
{
    if (stream == NULL)
        return;

    if (stream->pull_stream)
        net_delete_timer_task(stream->pull_stream);

    if (stream->push_stream)
        net_delete_timer_task(stream->push_stream);

    if (stream->scher)
        net_destroy_scheduler(stream->scher);

    if (stream->buffer)
        buffer_unint(stream->buffer);

    if (stream->fp)
        fclose(stream->fp);

    net_free(stream);
}

h264_stream *h264_stream_init(const char *file)
{
    int code = NET_FAIL;
    h264_stream * stream = NULL;

    do {
        stream = (h264_stream *)calloc(1, sizeof(h264_stream));
        if (stream == NULL)
            break;

        stream->fp = fopen(file, "rb+");
        if (stream->fp == NULL)
            break;

        stream->buffer = buffer_init(1024 * 15);
        if (stream->buffer == NULL)
            break;

        stream->scher = net_create_scheduler();
        if (stream->scher == NULL)
            break;
    
        stream->cache   = NULL;
        code = NET_SUCCESS;
    } while (0);

    if (code != NET_SUCCESS)
    {
        h264_stream_unint(stream);
        return NULL;
    }

    return stream;
}

int h264_stream_start(h264_stream *stream, shm_cache_ptr cache, server_ptr rtmp_server)
{
    if (stream == NULL)
        return NET_FAIL;

    stream->cache = cache;
    stream->rtmp_server = rtmp_server;
    
    stream->pull_stream = net_add_timer_task(stream->scher, 0, 10, rtmp_pull_h264_stream, (void *)stream);
    if (stream->pull_stream == NULL)
        return NET_FAIL;

    stream->push_stream = net_add_timer_task(stream->scher, 0, 10, rtmp_push_h264_stream, (void *)stream);
    if (stream->push_stream == NULL)
        return NET_FAIL;

    return NET_SUCCESS;   
}