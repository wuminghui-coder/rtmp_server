#include "rtmp-push-stream.h"
#include "rtmp-send-message.h"
#include "rtmp-h264-packet.h"
#include "rtmp-gop-cache.h"

typedef struct 
{
    playlive_ptr client;
    frame_package  *frame;
} push_stream;

//atomic_fetch_sub(&counter, 1);
//int old_value = atomic_exchange(&counter, 5);
// int expected = 5;
// int desired = 10;
// bool success = atomic_compare_exchange_strong(&counter, &expected, desired);
// // 如果 counter 的值等于 expected，则将其设置为 desired 并返回 true；否则，更新 expected 为 counter 的当前值并返回 false
// int value = atomic_load(&counter); // 读取 counter 的值
// atomic_store(&counter, 20); // 将 counter 的值设置为 20

int start_push_stream(void *data)
{
    push_stream * ps = (push_stream *)data;
    playlive_info * s = (playlive_info *)ps->client;
    long long timess = get_time_ms() - s->gtimess;
    //ERR("time: %lld, %d", timess, frame->type);
    s->gtimess += timess;
    rtmp_server_send_key_frame(s->stream, ps->frame->frame, ps->frame->size, s->times);
    s->times += 40;
    atomic_fetch_sub(&ps->frame->counter, 1);
    int value = atomic_load(&ps->frame->counter);
    if (value == 0)
    {
        net_free(ps->frame);
    }
    net_free(ps);
}

int rtmp_start_stream(void *stream, frame_package *frame)
{
    push_stream * ps = (push_stream *)calloc(1, sizeof(push_stream));
    ps->frame = frame;
    ps->client = stream;
    atomic_fetch_add(&frame->counter, 1);
    playlive_info * s   = (playlive_info *)ps->client;
    rtmp_ptr srtmp  = (rtmp_ptr)s->stream;
    net_add_trigger_task(srtmp->scher, start_push_stream, ps, 0);
}

int rtmp_start_push_stream(rtmp_ptr rtmp)
{
    playlive_info * s = (playlive_info *)calloc(1, sizeof(playlive_info));

    s->gtimess      = get_time_ms();
    s->times        = 1000;
    s->flags        = 0;
    s->stream       = rtmp;
    s->start_stream = rtmp_start_stream;
    s->type         = 1;

    gop_start_to_playlive(rtmp->gop, s);

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
