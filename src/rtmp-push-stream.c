#include "rtmp-push-stream.h"
#include "rtmp-send-message.h"
#include "rtmp-h264-packet.h"
#include "rtmp-gop-cache.h"

static long long gtimess = 0;

static int rtmp_push_h264_stream(void *args)
{
    // rtmp_ptr rtmp  = (rtmp_ptr)args;
    // if (rtmp == NULL || rtmp->cache == NULL)
    //    return NET_FAIL;

    // cache_buffer *b = shm_cache_get(rtmp->cache);
    // if (b == NULL)
    // {
    //     ERR("end-------------");
    //     return NET_FAIL;
    // }

    // if (rtmp->flags == 0 && b->type == NAL_UNIT_TYPE_SPS)
    // {
    //     net_modify_timer_task(rtmp->pull_stream, 10);
    //     rtmp->flags = 1;
    // }

    // if (rtmp->flags == 1)
    // {
    //     long long timess = get_time_ms() - gtimess;
    //     ERR("time: %lld", timess);
    //     gtimess += timess;
    //     //if (b->type == NAL_UNIT_TYPE_CODED_SLICE_IDR)
    //     rtmp_server_send_key_frame(rtmp, b->payload, b->size, rtmp->times);
    //     // if (b->type != NAL_UNIT_TYPE_CODED_SLICE_IDR)
    //     //     rtmp_server_send_inter_frame(rtmp, b->payload, b->size, 40);
    //     rtmp->times += 40;
    // }

    // net_free(b);
    return NET_SUCCESS;
}

int rtmp_start_stream(void *rtmp, frame_info *frame)
{
    rtmp_ptr rtmp1 = (rtmp_ptr)rtmp;
    long long timess = get_time_ms() - gtimess;
    ERR("time: %lld, %d", timess, type);
    rtmp_server_send_key_frame(rtmp1, frame->frame, frame->size, rtmp1->times);
    rtmp1->times += 40;
}

int rtmp_start_push_stream(rtmp_ptr rtmp)
{
    stream_info * s = (stream_info *)calloc(1, sizeof(stream_info));

    rtmp->times = 1000;
    rtmp->flags = 0;
    gtimess = get_time_ms();

    s->stream       = rtmp;
    s->start_stream = rtmp_start_stream;
    s->type  = 1;

    gop_start_to_playlive(rtmp->gop, s);

    // if (rtmp->pull_stream)
    //     return NET_FAIL;

    // rtmp->times = 1000;
    // rtmp->flags = 0;
    // gtimess = get_time_ms();

    // rtmp->pull_stream = net_add_timer_task(rtmp->scher, 0, 10, rtmp_push_h264_stream, (void *)rtmp);
    // if (rtmp->pull_stream == NULL)
    //     return NET_FAIL;

    return NET_SUCCESS;
}

void rtmp_stop_push_stream(rtmp_ptr rtmp)
{
    if (rtmp->pull_stream)
    {
        net_delete_timer_task(rtmp->pull_stream);  
        rtmp->pull_stream = NULL;
    }
}

