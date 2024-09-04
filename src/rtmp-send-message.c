#include "rtmp-send-message.h"
#include "rtmp-handshake.h"
#include "rtmp-recv-event.h"
#include "rtmp-chunk-header.h"
#include "rtmp-control-message.h"
#include "rtmp-write-packet.h"
#include "rtmp-send-reply.h"

int rtmp_server_send_handshake(rtmp_ptr rtmp)
{
    bs_reset(rtmp->send_buffer);

    rtmp_build_handshake_S0(rtmp->send_buffer, RTMP_VERSION);
    rtmp_build_handshake_S1(rtmp->send_buffer, time(NULL), NULL, RTMP_HANDSHAKE_SIZE);
    rtmp_build_handshake_S1(rtmp->send_buffer, time(NULL), begin_read(rtmp->buffer) + 1, RTMP_HANDSHAKE_SIZE);

    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_connect_result(rtmp_ptr rtmp, double transactionId)
{
    bs_reset(rtmp->send_buffer);

    uint8_t message[512] = {0};
    bs_t *mb = bs_new(message, sizeof(message));

    int length = rtmp_write_command_connect_reply(mb, transactionId , RTMP_FMSVER, 
                                    RTMP_CAPABILITIES, "NetConnection.Connect.Success", 
                                    RTMP_LEVEL_STATUS, "Connection succeeded.", 0.0);
    rtmp_chunk_write_header(rtmp->send_buffer, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_INVOKE, RTMP_TYPE_INVOKE, 0, length, 0);
    
    bs_clone_bs(rtmp->send_buffer, mb); 
    bs_free(mb);
    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_onmetadata(rtmp_ptr rtmp)
{
    bs_reset(rtmp->send_buffer);

    uint8_t message[512] = {0};
    bs_t *mb = bs_new(message, sizeof(message));
    int length = rtmp_write_onMetadata(mb);
    rtmp_chunk_write_header(rtmp->send_buffer, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_VIDEO, RTMP_TYPE_DATA, 0, length, 0);
    
    bs_clone_bs(rtmp->send_buffer, mb); 
    bs_free(mb);
    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_acknowledgement(rtmp_ptr rtmp, uint32_t acknowledgement)
{
    bs_reset(rtmp->send_buffer);
    
    rtmp_window_acknowledgement_size(rtmp->send_buffer, acknowledgement);
    
    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_chunk_size(rtmp_ptr rtmp, int chunk_size)
{
    bs_reset(rtmp->send_buffer);
    
    rtmp_set_chunk_size(rtmp->send_buffer, chunk_size);
    
    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_peer_bandwidth(rtmp_ptr rtmp, uint32_t window_size, uint8_t limit_type)
{
    bs_reset(rtmp->send_buffer);
    
    rtmp_set_peer_bandwidth(rtmp->send_buffer, window_size, limit_type);
    
    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_create_stream_result(rtmp_ptr rtmp, double transactionId)
{
    bs_reset(rtmp->send_buffer);

    uint8_t message[512] = {0};
    bs_t *mb = bs_new(message, sizeof(message));

    int length = rtmp_write_command_stream_reply(mb, transactionId, rtmp->packet->header.stream_id);
    rtmp_chunk_write_header(rtmp->send_buffer, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_INVOKE, RTMP_TYPE_INVOKE, 0, length, 0);
    
    bs_clone_bs(rtmp->send_buffer, mb); 
    bs_free(mb);
    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_stream_begin(rtmp_ptr rtmp, uint32_t streamId)
{
    bs_reset(rtmp->send_buffer);

    rtmp_set_stream_begin(rtmp->send_buffer, streamId);

    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_stream_is_record(rtmp_ptr rtmp, uint32_t streamId)
{
    bs_reset(rtmp->send_buffer);

    rtmp_set_stream_is_record(rtmp->send_buffer, streamId);

    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_sample_access(rtmp_ptr rtmp, uint32_t streamId)
{
    bs_reset(rtmp->send_buffer);

    uint8_t message[512] = {0};
    bs_t *mb = bs_new(message, sizeof(message));

    int length = rtmp_write_rtmpsampleaccess(mb);
    rtmp_chunk_write_header(rtmp->send_buffer, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_DATA, RTMP_TYPE_DATA, streamId, length, 0);
    
    bs_clone_bs(rtmp->send_buffer, mb); 
    bs_free(mb);

    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_servrt_send_onstatus(rtmp_ptr rtmp, uint32_t streamId, double transactionId, const char *level, const char *code, const char *description)
{
    bs_reset(rtmp->send_buffer);
    
    uint8_t message[512] = {0};
    bs_t *mb = bs_new(message, sizeof(message));

    int length = rtmp_write_onstatus(mb, transactionId, level, code, description);
    rtmp_chunk_write_header(rtmp->send_buffer, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_INVOKE, RTMP_TYPE_INVOKE, 0, length, 0);
    
    bs_clone_bs(rtmp->send_buffer, mb); 
    bs_free(mb);

    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_onget_stream_lenght(rtmp_ptr rtmp, double transactionId, double duration)
{
    bs_reset(rtmp->send_buffer);
    
    uint8_t message[512] = {0};
    bs_t *mb = bs_new(message, sizeof(message));

    int length = rtmp_write_command_stream_reply(mb, transactionId, duration);

    rtmp_chunk_write_header(rtmp->send_buffer, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_INVOKE, RTMP_TYPE_INVOKE, 0, length, 0);
    
    bs_clone_bs(rtmp->send_buffer, mb); 
    bs_free(mb);

    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_stream_seek(rtmp_ptr rtmp, double transactionId, double timestamp)
{
    bs_reset(rtmp->send_buffer);
    
    uint8_t message[512] = {0};
    bs_t *mb = bs_new(message, sizeof(message));

    int length = rtmp_write_seek(mb,  transactionId, timestamp);
    rtmp_chunk_write_header(rtmp->send_buffer, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_INVOKE, RTMP_TYPE_INVOKE, rtmp->packet->header.stream_id, length, 0);
    
    bs_clone_bs(rtmp->send_buffer, mb); 
    bs_free(mb);

    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);

}

int rtmp_server_send_connect(rtmp_ptr rtmp, double transactionId, uint32_t streamId)
{
    bs_reset(rtmp->send_buffer);

    uint8_t message[512] = {0};
    bs_t *mb = bs_new(message, sizeof(message));

    int length = rtmp_write_command_connect(mb, transactionId, &rtmp->conn);

    rtmp_chunk_write_header(rtmp->send_buffer, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_INVOKE, RTMP_TYPE_INVOKE, streamId, length, 0);
    
    bs_clone_bs(rtmp->send_buffer, mb); 
    bs_free(mb);
    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

int rtmp_server_send_release_stream(rtmp_ptr rtmp, double transactionId, const char* stream_name)
{
    bs_reset(rtmp->send_buffer);
    
    uint8_t message[512] = {0};
    bs_t *mb = bs_new(message, sizeof(message));

    int length = rtmp_write_release_stream(mb,  transactionId, stream_name);
    rtmp_chunk_write_header(rtmp->send_buffer, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_INVOKE, RTMP_TYPE_INVOKE, rtmp->packet->header.stream_id, length, 0);
    
    bs_clone_bs(rtmp->send_buffer, mb); 
    bs_free(mb);

    return send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
}

// // FCPublish
// int rtmp_client_send_fcpublish(rtmp_ptr rtmp)
// {
// 	int r;
// 	r = (int)(rtmp_netstream_fcpublish(ctx->payload, sizeof(ctx->payload), 0, ctx->stream_name) - ctx->payload);
// 	return rtmp_client_send_control(&ctx->rtmp, ctx->payload, r, 0);
// }

// // FCUnpublish
// int rtmp_client_send_fcunpublish(rtmp_ptr rtmp)
// {
// 	int r;
// 	r = (int)(rtmp_netstream_fcunpublish(ctx->payload, sizeof(ctx->payload), 0, ctx->stream_name) - ctx->payload);
// 	return rtmp_client_send_control(&ctx->rtmp, ctx->payload, r, ctx->stream_id);
// }

// // createStream
// int rtmp_client_send_create_stream(rtmp_ptr rtmp)
// {
// 	int r;
// 	assert(0 == ctx->stream_id);
// 	r = (int)(rtmp_netconnection_create_stream(ctx->payload, sizeof(ctx->payload), RTMP_TRANSACTION_CREATE_STREAM) - ctx->payload);
// 	return rtmp_client_send_control(&ctx->rtmp, ctx->payload, r, 0);
// }

// // deleteStream
// int rtmp_client_send_delete_stream(rtmp_ptr rtmp)
// {
// 	int r;
// 	assert(0 != ctx->stream_id);
// 	r = (int)(rtmp_netstream_delete_stream(ctx->payload, sizeof(ctx->payload), 0, ctx->stream_id) - ctx->payload);
// 	return rtmp_client_send_control(&ctx->rtmp, ctx->payload, r, ctx->stream_id);
// }

// // publish
// int rtmp_client_send_publish(rtmp_ptr rtmp)
// {
// 	int r;
// 	assert(0 != ctx->stream_id);
// 	r = (int)(rtmp_netstream_publish(ctx->payload, sizeof(ctx->payload), 0, ctx->stream_name, RTMP_STREAM_LIVE) - ctx->payload);
// 	return rtmp_client_send_control(&ctx->rtmp, ctx->payload, r, ctx->stream_id);
// }

// // play
// int rtmp_client_send_play(rtmp_ptr rtmp)
// {
// 	int r;
// 	assert(0 != ctx->stream_id);
// 	r = (int)(rtmp_netstream_play(ctx->payload, sizeof(ctx->payload), 0, ctx->stream_name, -2, -1, 1) - ctx->payload);
// //	rtmp_client_chunk_header_default(&header, RTMP_CHANNEL_CONTROL, (uint32_t)time(NULL), r, RTMP_TYPE_INVOKE, ctx->stream_id);
// 	return rtmp_client_send_control(&ctx->rtmp, ctx->payload, r, ctx->stream_id);
// }

int rtmp_server_send_audio(rtmp_ptr rtmp, const void* data, uint32_t length, uint32_t timestamp)
{
    if (rtmp == NULL)
        return NET_FAIL;

    rtmp_chunk_header header = {
        .csid = RTMP_CHANNEL_AUDIO,
        .fmt  = RTMP_CHUNK_TYPE_1,
        .length = length,
        .stream_id = 0,
        .type_id = RTMP_TYPE_AUDIO,
        .timestamp = timestamp
    };

    return rtmp_write_packet(rtmp, &header, (uint8_t *)data);
}

int rtmp_server_send_key_frame(rtmp_ptr rtmp, const void* data, uint32_t length, uint32_t timestamp)
{
    if (rtmp == NULL)
        return NET_FAIL;

    rtmp_chunk_header header = {
        .csid = RTMP_CHANNEL_DATA,
        .fmt  = RTMP_CHUNK_TYPE_0,
        .length = length,
        .stream_id = 0,
        .type_id = RTMP_TYPE_VIDEO,
        .timestamp = timestamp
    };

    return rtmp_write_packet(rtmp, &header, (uint8_t *)data);
}

int rtmp_server_send_inter_frame(rtmp_ptr rtmp, const void* data, uint32_t length, uint32_t timestamp)
{
    if (rtmp == NULL)
        return NET_FAIL;

    rtmp_chunk_header header = {
        .csid = RTMP_CHANNEL_DATA1,
        .fmt  = RTMP_CHUNK_TYPE_1,
        .length = length,
        .stream_id = 0,
        .type_id = RTMP_TYPE_VIDEO,
        .timestamp = timestamp
    };

    return rtmp_write_packet(rtmp, &header, (uint8_t *)data);
}

int rtmp_server_send_script(rtmp_ptr rtmp, const void* data, uint32_t length, uint32_t timestamp)
{
    if (rtmp == NULL)
        return NET_FAIL;
        
    rtmp_chunk_header header = {
        .csid = RTMP_CHANNEL_INVOKE,
        .fmt  = RTMP_CHUNK_TYPE_1,
        .length = length,
        .stream_id = 0,
        .type_id = RTMP_TYPE_DATA,
        .timestamp = timestamp
    };

    return rtmp_write_packet(rtmp, &header, (uint8_t *)data);
}




