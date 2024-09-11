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

static void rtmp_paser_packet(h264_stream * stream, uint8_t *data, int size, int type)
{
    if (stream->ring_cache && data)
        shm_cache_put(stream->ring_cache, data, size, type);
}

static int _rtmp_pull_h264_stream(void *args)
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

    if (stream->buffer)
        buffer_unint(stream->buffer);

    if (stream->fp)
        fclose(stream->fp);

    net_free(stream);
}

static int _rtmp_push_h264_stream(void *args)
{
    h264_stream * stream  = (h264_stream *)args;
    if (stream == NULL || stream->ring_cache == NULL)
       return NET_FAIL;

    cache_buffer *b = shm_cache_get(stream->ring_cache);
    if (b == NULL)
        return NET_FAIL;

    frame_package *f = new_frame_package(b->type, b->size, b->payload, b->size);

    if (b->type == NAL_UNIT_TYPE_SPS)
    {
        gop_set_sps(stream->gop, f);
    }
    else if(b->type == NAL_UNIT_TYPE_PPS)
    {
        gop_set_pps(stream->gop, f);
    }
    else {
        gop_pull_frame_to_cache(stream->gop, f);
    }

    net_free(b);
    return NET_SUCCESS;
}

h264_stream *h264_stream_init(const char *file, rtmp_gop *gop)
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

        stream->ring_cache = shm_cache_init(4096 * 1000 * 100);

        stream->scher = net_create_scheduler();

        stream->gop = gop;

        code = NET_SUCCESS;
    } while (0);

    if (code != NET_SUCCESS)
    {
        h264_stream_unint(stream);
        return NULL;
    }

    return stream;
}

void h264_start_stream(h264_stream * stream)
{
    stream->pull_stream = net_add_timer_task(stream->scher, 0, 10, _rtmp_pull_h264_stream, (void *)stream);
    if (stream->pull_stream == NULL)
        return ;

    stream->push_stream = net_add_timer_task(stream->scher, 0, 40, _rtmp_push_h264_stream, (void *)stream);
    if (stream->push_stream == NULL)
        return ;
}