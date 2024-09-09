#include "rtmp-control-message.h"
#include "rtmp-chunk-header.h"
#include "rtmp-recv-event.h"

int rtmp_set_chunk_size(bs_t *b, int chunk_size)
{
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_SET_CHUNK_SIZE, 0, 4, 0);
    
    bs_write_u(b, 32, chunk_size & 0x7FFFFFFF);
    
    return bs_pos(b) - start_pos;
}

int rtmp_set_abort(bs_t *b, uint32_t chunk_streamid)
{
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_ABORT, 0, 4, 0);
    
    bs_write_u(b, 32, chunk_streamid);
    
    return bs_pos(b) - start_pos;
}

int rtmp_set_acknowledgement(bs_t *b, uint32_t sequence_number)
{ 
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_ACKNOWLEDGEMENT, 0, 4, 0);
    
    bs_write_u(b, 32, sequence_number);
    
    return bs_pos(b) - start_pos;
}

int rtmp_window_acknowledgement_size(bs_t *b, uint32_t window_size)
{ 
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_WINDOW_ACKNOWLEDGEMENT_SIZE, 0, 4, 0);
    
    bs_write_u(b, 32, window_size);
    
    return bs_pos(b) - start_pos;
}

int rtmp_set_peer_bandwidth(bs_t *b, uint32_t window_size, uint8_t limit_type)
{
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_SET_PEER_BANDWIDTH, 0, 5, 0);
    
    bs_write_u(b, 32, window_size);
    bs_write_u8(b, limit_type);

    return bs_pos(b) - start_pos;
}


int rtmp_set_stream_begin(bs_t *b, uint32_t streamId)
{
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_EVENT, 0, 6, 0);

    bs_write_u(b, 16, RTMP_EVENT_STREAM_BEGIN);
    bs_write_u(b, 32, streamId);

	return bs_pos(b) - start_pos;
}

int rtmp_set_stream_eof(bs_t *b, uint32_t streamId)
{
	if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_EVENT, 0, 6, 0);

    bs_write_u(b, 16, RTMP_EVENT_STREAM_EOF);
    bs_write_u(b, 32, streamId);

	return bs_pos(b) - start_pos;
}


int rtmp_set_stream_dry(bs_t *b, uint32_t streamId)
{
	if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_EVENT, 0, 6, 0);

    bs_write_u(b, 16, RTMP_EVENT_STREAM_DRY);
    bs_write_u(b, 32, streamId);

	return bs_pos(b) - start_pos;
}

int rtmp_set_set_buffer_length(bs_t *b, uint32_t streamId, uint32_t ms)
{
	
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_EVENT, 0, 10, 0);

    bs_write_u(b, 16, RTMP_EVENT_SET_BUFFER_LENGTH);
    bs_write_u(b, 32, streamId);
    bs_write_u(b, 32, ms);

	return bs_pos(b) - start_pos;
}

int rtmp_set_stream_is_record(bs_t *b, uint32_t streamId)
{
	if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_EVENT, 0, 6, 0);

    bs_write_u(b, 16, RTMP_EVENT_STREAM_IS_RECORD);
    bs_write_u(b, 32, streamId);

	return bs_pos(b) - start_pos;
}

int rtmp_set_ping(bs_t *b, uint32_t timstamp)
{
	if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_EVENT, 0, 6, 0);

    bs_write_u(b, 16, RTMP_EVENT_PING);
    bs_write_u(b, 32, timstamp);

	return bs_pos(b) - start_pos;
}

int rtmp_set_pong(bs_t *b, uint32_t timstamp)
{
	if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    rtmp_chunk_write_header(b, RTMP_CHUNK_TYPE_0, RTMP_CHANNEL_PROTOCOL, RTMP_TYPE_EVENT, 0, 6, 0);

    bs_write_u(b, 16, RTMP_EVENT_PONG);
    bs_write_u(b, 32, timstamp);

	return bs_pos(b) - start_pos;
}
