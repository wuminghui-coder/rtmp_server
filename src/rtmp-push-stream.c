#include "rtmp-push-stream.h"
#include "rtmp-send-message.h"
#include "rtmp-h264-packet.h"
#include "rtmp-gop-cache.h"



int rtmp_start_stream(void *stream, frame_info *frame)
{
    stream_info * s = (stream_info *)stream;

    long long timess = get_time_ms() - s->gtimess;
    //ERR("time: %lld, %d", timess, frame->type);
    s->gtimess += timess;
    rtmp_server_send_key_frame(s->stream, frame->frame, frame->size, s->times);

    s->times += 40;
}

int rtmp_start_push_stream(rtmp_ptr rtmp)
{
    stream_info * s = (stream_info *)calloc(1, sizeof(stream_info));

    s->gtimess = get_time_ms();
    s->times        = 1000;
    s->flags        = 0;
    s->stream       = rtmp;
    s->start_stream = rtmp_start_stream;
    s->type         = 1;

    gop_start_to_playlive(rtmp->gop, s);

    return NET_SUCCESS;
}

