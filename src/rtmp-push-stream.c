#include "rtmp-push-stream.h"
#include "rtmp-send-message.h"
#include "rtmp-h264-packet.h"
#include "rtmp-gop-cache.h"

typedef struct 
{
    frame_package *frame;
    playlive_ptr client;
} stream_info;

static int _start_push_stream(void *data)
{
    if (!data)
        return NET_FAIL;

    stream_info * stream = (stream_info *)data;

    frame_package *frame = (frame_package *)stream->frame;
    playlive_ptr client = (playlive_ptr)stream->client;

    rtmp_server_send_key_frame(client->service, frame->frame, frame->size, client->base_time);
    client->base_time += client->interval;
    long long sss = client->interval_test;
    client->interval_test = get_time_ms();
    //ERR("----------%lld,  %lld", client->interval_test - sss, client->base_time);
    frame_package_release(frame);
    net_free(stream);
    return NET_SUCCESS;
}

static void _rtmp_start_stream(void *client, frame_package *frame)
{
    if (!client || !frame)
        return;

    rtmp_ptr rtmp = (rtmp_ptr) ((playlive_ptr)client)->service;
    if (!rtmp)
        return;

    stream_info * stream = (stream_info *)calloc(1, sizeof(stream_info));
    if (stream == NULL)
        return;

    stream->client = client;
    stream->frame  = frame;

    frame_package_count(frame);

    net_add_trigger_task(rtmp->scher, _start_push_stream, stream, 0);
}

int rtmp_start_push_stream(rtmp_ptr rtmp)
{
    if (!rtmp || !rtmp->gop)
        return NET_FAIL;

    playlive_ptr client = new_playlive(rtmp, 33, _rtmp_start_stream);
    if (client == NULL)
        return NET_FAIL;
  
    rtmp->client = client;

    gop_start_to_playlive(rtmp->gop, client);

    return NET_SUCCESS;
}

frame_package *rtmp_write_frame(frame_package *frame)
{
    if (frame == NULL)
        return NULL;

    int frame_length = RTMP_FRAME_HEADER_LENGTH + frame->size;

    frame_package *f = new_frame_package(frame->type, frame_length, NULL, 0);
    if (f == NULL)
        return NULL;
        
    bs_t *b = bs_new(f->frame, frame_length);
    if (b == NULL)
    {
        net_free(f);
        return NULL;
    }

    if (NAL_UNIT_TYPE_CODED_SLICE_IDR == frame->type)
        bs_write_u8(b, 0x17); //0x27
    else 
        bs_write_u8(b, 0x27); //0x27
    
    bs_write_u8(b, 0x01);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);

    bs_write_u(b, 32, frame->size);

    bs_write_bytes(b, frame->frame, frame->size);
    
    bs_free(b);

    return f;
}

frame_package *rtmp_write_avc_sequence(frame_package *sps, frame_package *pps)
{
    if (sps == NULL || pps == NULL)
        return NULL;

    int frame_length = sps->size + pps->size + RTMP_AVC_HEADER_LENGTH;

    frame_package *f = new_frame_package(sps->type, frame_length, NULL, 0);
    if (f == NULL)
        return NULL;

    bs_t *b = bs_new(f->frame, frame_length);
    if (b == NULL)
    {
        net_free(f);
        return NULL;
    }

    bs_write_u8(b, 0x17);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);
    bs_write_u8(b, 0x00);

    bs_write_u8(b, 0x01);
    bs_write_u8(b, sps->frame[1]);
    bs_write_u8(b, sps->frame[2]);
    bs_write_u8(b, sps->frame[3]);
    bs_write_u8(b, 0xff);

    bs_write_u8(b, 0xe1);
    bs_write_u(b, 16, sps->size);
    bs_write_bytes(b, sps->frame, sps->size);
    
    bs_write_u8(b, 0x01);
    bs_write_u(b, 16, pps->size);
    bs_write_bytes(b, pps->frame, pps->size);

    bs_free(b);
    return f;
}
