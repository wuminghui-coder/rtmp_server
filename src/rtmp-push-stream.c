#include "rtmp-push-stream.h"
#include "rtmp-send-message.h"
#include "rtmp-h264-packet.h"
#include "rtmp-gop-cache.h"

typedef struct 
{
    playlive_ptr client;
    frame_package  *frame;
} push_stream;

static int _start_push_stream(void *data)
{
    if (!data)
        return NET_FAIL;
    push_stream *stream = (push_stream *)data;
    playlive_info * client = (playlive_info *)stream->client;

    rtmp_server_send_key_frame(client->service, stream->frame->frame, stream->frame->size, client->interval);
    frame_package_release(stream->frame);
    net_free(stream);

    long long interval = client->base_time;
    client->base_time = get_time_ms();
    //ERR("time: %lld", client->base_time - interval);
    client->interval += 40;
    return NET_SUCCESS;
}

static void _rtmp_start_stream(void *client, frame_package *frame)
{
    if (!client || !frame)
        return;

    push_stream * stream = (push_stream *)calloc(1, sizeof(push_stream));
    if (!stream)
        return;
    stream->frame  = frame;
    stream->client = client;
    frame_package_count(frame);
    rtmp_ptr rtmp = (rtmp_ptr)stream->client->service;
    net_add_trigger_task(rtmp->scher, _start_push_stream, stream, 0);
}

int rtmp_start_push_stream(rtmp_ptr rtmp)
{
    if (!rtmp)
        return NET_FAIL;

    playlive_ptr client = (playlive_ptr)calloc(1, sizeof(playlive_info));
    if (!client)
        return NET_FAIL;

    client->base_time = get_time_ms();
    client->interval = 1000;
    client->service = rtmp;
    client->start_stream = _rtmp_start_stream;
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
