#ifndef __RTMP_SERVER_SEND_H__
#define __RTMP_SERVER_SEND_H__
#include "net-common.h"
#include "rtmp-session.h"

enum {
    RTMP_BANDWIDTH_LIMIT_HARD = 0,
    RTMP_BANDWIDTH_LIMIT_SOFT = 1,
    RTMP_BANDWIDTH_LIMIT_DYNAMIC = 2,
};


int rtmp_server_send_handshake(rtmp_ptr rtmp);
int rtmp_server_send_connect_result(rtmp_ptr rtmp, double transactionId);
int rtmp_server_send_create_stream_result(rtmp_ptr rtmp, double transactionId);

int rtmp_server_send_chunk_size(rtmp_ptr rtmp, int chunk_size);
int rtmp_server_send_acknowledgement(rtmp_ptr rtmp, uint32_t acknowledgement);
int rtmp_server_send_peer_bandwidth(rtmp_ptr rtmp, uint32_t window_size, uint8_t limit_type);

int rtmp_server_send_stream_begin(rtmp_ptr rtmp, uint32_t streamId);
int rtmp_servrt_send_onstatus(rtmp_ptr rtmp, uint32_t streamId, double transactionId, const char *level, const char *code, const char *description);
int rtmp_server_send_stream_is_record(rtmp_ptr rtmp, uint32_t streamId);
int rtmp_server_send_onget_stream_lenght(rtmp_ptr rtmp, double transactionId, double duration);
int rtmp_server_send_sample_access(rtmp_ptr rtmp, uint32_t streamId);
int rtmp_server_send_stream_seek(rtmp_ptr rtmp, double transactionId, double timestamp);
//int rtmp_server_send_video(rtmp_ptr rtmp, const void* data, uint32_t length, uint32_t timestamp);
int rtmp_server_send_onmetadata(rtmp_ptr rtmp);
int rtmp_server_send_inter_frame(rtmp_ptr rtmp, const void* data, uint32_t length, uint32_t timestamp);
int rtmp_server_send_key_frame(rtmp_ptr rtmp, const void* data, uint32_t length, uint32_t timestamp);

#endif // !__RTMP_SERVER_SEND_H__
