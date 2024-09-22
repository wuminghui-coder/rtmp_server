#include "rtmp-send-reply.h"
#include "rtmp-send-message.h"
#include "rtmp-push-stream.h"
int rtmp_reply_connect(rtmp_ptr rtmp, int code, double transactionId)
{
    if (rtmp == NULL)
        return NET_FAIL;

    rtmp_server_send_peer_bandwidth(rtmp, RTMP_WINDOW_SIZE, RTMP_BANDWIDTH_LIMIT_DYNAMIC);

    rtmp_server_send_acknowledgement(rtmp, RTMP_WINDOW_SIZE);

    rtmp_server_send_chunk_size(rtmp, RTMP_OUTPUT_CHUNK_SIZE);

    return rtmp_server_send_connect_result(rtmp, transactionId);
}

int rtmp_reply_result(rtmp_ptr rtmp, int code, double transactionId)
{
    if (rtmp == NULL)
        return NET_FAIL;
    return rtmp_server_send_create_stream_result(rtmp, transactionId);
}

//rtmp_servrt_send_onstatus(rtmp, rtmp->packet->header.stream_id, transactionId, RTMP_LEVEL_STATUS, "NetStream.Play.Reset", "Resetting and playing stream");
// rtmp_server_send_onstatus(rtmp, "NetStream.Pause.Notify" : "NetStream.Unpause.Notify", "NetStream.Pause.Failed", "");
// rtmp_server_send_onstatus(rtmp, "NetStream.DeleteStream.Suceess", "NetStream.DeleteStream.Failed", "");
// rtmp_server_send_onstatus(rtmp, "NetStream.Publish.Start", "NetStream.Publish.BadName", "");
// rtmp_server_send_onstatus(rtmp, "NetStream.Seek.Notify", "NetStream.Seek.Failed", "");
// rtmp_server_send_onstatus(rtmp, "NetStream.Play.Start", "NetStream.Play.Failed", "");
int rtmp_reply_onplay(rtmp_ptr rtmp, int code, double transactionId)
{
    if (rtmp == NULL)
        return NET_FAIL;

    rtmp_server_send_stream_begin(rtmp, rtmp->packet->header.stream_id);

    char message[140] = {0};
    sprintf(message, "start %s", rtmp->conn.app);
    rtmp_servrt_send_onstatus(rtmp, rtmp->packet->header.stream_id, transactionId, RTMP_LEVEL_STATUS, "NetStream.Play.Start", message);

    rtmp_server_send_sample_access(rtmp, rtmp->packet->header.stream_id);

    rtmp_server_send_onmetadata(rtmp);

    return rtmp_start_push_stream(rtmp);
}

int rtmp_reply_onstatus(rtmp_ptr rtmp, int code, double transactionId)
{
    if (rtmp == NULL)
        return NET_FAIL;

    char message[140] = {0};
    sprintf(message, "start %s", rtmp->conn.app);
    return rtmp_servrt_send_onstatus(rtmp, rtmp->packet->header.stream_id, transactionId, RTMP_LEVEL_STATUS, "NetStream.Publish.Start", message);
}