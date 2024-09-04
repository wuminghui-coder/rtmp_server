#include "rtmp-recv-event.h"
#include "amf0.h"
#include "rtmp-read-invoke.h"
#include "rtmp-send-message.h"

int rtmp_recv_paser_event(rtmp_ptr rtmp, bs_t *b)
{
    if (rtmp == NULL || b == NULL)
        return NET_FAIL;
    int event_type = bs_read_u(b, 16);
    if (event_type == SetBufferLength)
    {    
        int stream_id    = bs_read_u(b, 32);
        int cache_lenth  = bs_read_u(b, 32);
        LOG("event_type %d,%d,%d", event_type, stream_id, cache_lenth);
    }
    //rtmp_server_stream_begin(rtmp_ptr rtmp, uint32_t streamId);
}

int rtmp_recv_paser_acknowledgement(rtmp_session *rtmp, bs_t *b)
{
    if (rtmp == NULL || b == NULL)
        return NET_FAIL;
    
    int window_size  = bs_read_u(b, 32);

    LOG("window size %d", window_size);

    return NET_SUCCESS;
}

int rtmp_recv_event(rtmp_session *rtmp)
{
    if (rtmp == NULL || rtmp->packet == NULL)
        return NET_FAIL;

    bs_t *b = bs_new(rtmp->packet->ployload, rtmp->packet->length);
    if (b == NULL)
        return NET_FAIL;

    LOG("type id %d, length %d", rtmp->packet->header.type_id, rtmp->packet->length);

    switch (rtmp->packet->header.type_id)
    {
        case RTMP_TYPE_FLEX_MESSAGE:

        case RTMP_TYPE_INVOKE:
            rtmp_recv_paser_invoke(rtmp, b);
            break;
        case RTMP_TYPE_VIDEO:

        case RTMP_TYPE_AUDIO:

        case RTMP_TYPE_EVENT:
            rtmp_recv_paser_event(rtmp, b);
            break;
        case RTMP_TYPE_SET_CHUNK_SIZE:
        case RTMP_TYPE_ABORT:
        case RTMP_TYPE_ACKNOWLEDGEMENT:
        case RTMP_TYPE_WINDOW_ACKNOWLEDGEMENT_SIZE:
            rtmp_recv_paser_acknowledgement(rtmp, b);
            break;
        case RTMP_TYPE_SET_PEER_BANDWIDTH:

        case RTMP_TYPE_DATA:
        case RTMP_TYPE_FLEX_STREAM:

        case RTMP_TYPE_SHARED_OBJECT:
        case RTMP_TYPE_FLEX_OBJECT:

        case RTMP_TYPE_METADATA:

        default:
            break;
    }

    bs_free(b);
    net_free(rtmp->packet->ployload);
    net_free(rtmp->packet);
    return NET_SUCCESS;
}
