#include "rtmp-recv-event.h"
#include "amf0.h"
#include "rtmp-read-invoke.h"
#include "rtmp-send-message.h"

static int rtmp_recv_paser_event(rtmp_ptr rtmp, bs_t *b)
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
    return NET_SUCCESS;
}

static int rtmp_recv_paser_acknowledgement(rtmp_session *rtmp, bs_t *b)
{
    if (rtmp == NULL || b == NULL)
        return NET_FAIL;
    
    int window_size  = bs_read_u(b, 32);

    LOG("window size %d", window_size);

    return NET_SUCCESS;
}

static int rtmp_recv_set_chunk_size(rtmp_session *rtmp, bs_t *b)
{
    if (rtmp == NULL || b == NULL)
        return NET_FAIL;

    int chunk_size = bs_read_u(b, 32);

    LOG("chunk_size %d", chunk_size);

    return NET_SUCCESS;
}

static int rtmp_recv_set_onMetaData(rtmp_session *rtmp, bs_t *b)
{
    if (rtmp == NULL || b == NULL)
        return NET_FAIL;

    amf_object_item items[1];
    amf_object_item items_common[2];
    amf_object_item commands[18];

    int duration = 0;
    int width    = 0;
    int height   = 0;
    int videodatarate = 0;
    int framerate = 0;
    int videocodecid = 0;
    int audiodatarate = 0;
    int audiosamplerate = 0;
    int audiosamplesize = 0;
    int stereo = 0;
    int audiocodecid = 0;
    char major_brand[32] = {0};
    char minor_version[32] = {0};
    char compatible_brands[32] = {0};
    char encoder[32] = {0};
    int filesize = 0;
    char setDataFrame[32] = {0};
    char onMetaData[32] = {0};

    AMF_OBJECT_ITEM_VALUE(items_common[0], AMF_STRING, "@setDataFrame", setDataFrame, sizeof(setDataFrame));
    AMF_OBJECT_ITEM_VALUE(items_common[1], AMF_STRING, "onMetaData", onMetaData, sizeof(onMetaData));

    AMF_OBJECT_ITEM_VALUE(commands[0], AMF_NUMBER, "duration",      &duration,    8);
    AMF_OBJECT_ITEM_VALUE(commands[1], AMF_NUMBER, "width",         &width,       8);
    AMF_OBJECT_ITEM_VALUE(commands[2], AMF_NUMBER, "height",        &height,      8);
    AMF_OBJECT_ITEM_VALUE(commands[3], AMF_NUMBER, "videodatarate", &videodatarate, 8);
    AMF_OBJECT_ITEM_VALUE(commands[4], AMF_NUMBER, "framerate",     &framerate,    8);
    AMF_OBJECT_ITEM_VALUE(commands[5], AMF_NUMBER, "videocodecid",  &videocodecid, 8);
    AMF_OBJECT_ITEM_VALUE(commands[6], AMF_NUMBER, "audiodatarate", &audiodatarate, 8);
    AMF_OBJECT_ITEM_VALUE(commands[7], AMF_NUMBER, "audiosamplerate", &audiosamplerate, 8);
    AMF_OBJECT_ITEM_VALUE(commands[8], AMF_NUMBER, "audiosamplesize", &audiosamplesize, 8);

    AMF_OBJECT_ITEM_VALUE(commands[9],  AMF_BOOLEAN, "stereo",        &stereo, 1);
    AMF_OBJECT_ITEM_VALUE(commands[10],  AMF_NUMBER,  "audiocodecid",  &audiocodecid, 8);
    AMF_OBJECT_ITEM_VALUE(commands[11], AMF_STRING,  "major_brand",   major_brand, sizeof(major_brand));
    AMF_OBJECT_ITEM_VALUE(commands[12], AMF_STRING,  "minor_version", minor_version, sizeof(minor_version));
    AMF_OBJECT_ITEM_VALUE(commands[13], AMF_STRING,  "compatible_brands", compatible_brands, sizeof(compatible_brands));
    AMF_OBJECT_ITEM_VALUE(commands[14], AMF_STRING,  "encoder",       encoder, sizeof(encoder));
    AMF_OBJECT_ITEM_VALUE(commands[15], AMF_NUMBER,  "filesize",      &filesize, 8);

    AMF_OBJECT_ITEM_VALUE(items[2], AMF_ECMA_ARRAY, "command", commands, sizeof(commands) / sizeof(commands[0]));

    amf_read_item(b, items_common, sizeof(items_common) / sizeof(items_common[0]));

    return amf_read_object_item(b, bs_read_u8(b), items);
}


static int rtmp_recv_set_peer_bandwidth(rtmp_session *rtmp, bs_t *b)
{
    if (rtmp == NULL || b == NULL)
        return NET_FAIL;
    int window     = bs_read_u(b, 32);
    int limit_type = bs_read_u(b, 8);
    ERR("window %d, limit type %d", window, limit_type);
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
            rtmp_recv_set_chunk_size(rtmp, b);
            break;
        case RTMP_TYPE_ABORT:
        case RTMP_TYPE_ACKNOWLEDGEMENT:
        case RTMP_TYPE_WINDOW_ACKNOWLEDGEMENT_SIZE:
            rtmp_recv_paser_acknowledgement(rtmp, b);
            break;
        case RTMP_TYPE_SET_PEER_BANDWIDTH:
            rtmp_recv_set_peer_bandwidth(rtmp, b);
            break;
        case RTMP_TYPE_DATA:
            rtmp_recv_set_onMetaData(rtmp, b);
            break;
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
