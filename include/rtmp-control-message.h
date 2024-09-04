#ifndef __RTMP_CONTROL_MESSAGE_H__
#define __RTMP_CONTROL_MESSAGE_H__
#include "net-common.h"

enum
{
	RTMP_AMF_EVENT_USE				= 1,
	RTMP_AMF_EVENT_RELEASE			= 2,
	RTMP_AMF_EVENT_REQUEST_CHANGE	= 3,
	RTMP_AMF_EVENT_CHANGE			= 4,
	RTMP_AMF_EVENT_SUCCESS			= 5,
	RTMP_AMF_EVENT_SEND_MESSAGE		= 6,
	RTMP_AMF_EVENT_STATUS			= 7,
	RTMP_AMF_EVENT_CLEAR			= 8,
	RTMP_AMF_EVENT_REMOVE			= 9,
	RTMP_AMF_EVENT_REQUEST_REMOVE	= 10,
	RTMP_AMF_EVENT_USE_SUCCESS		= 11,
};

// 7.1.7. User Control Message Events (p27)
enum
{
	RTMP_EVENT_STREAM_BEGIN			= 0,
	RTMP_EVENT_STREAM_EOF			= 1,
	RTMP_EVENT_STREAM_DRY			= 2,
	RTMP_EVENT_SET_BUFFER_LENGTH	= 3,
	RTMP_EVENT_STREAM_IS_RECORD		= 4,

	RTMP_EVENT_PING					= 6, // RTMP_EVENT_PING_REQUEST
	RTMP_EVENT_PONG					= 7, // RTMP_EVENT_PING_RESPONSE

	// https://www.gnu.org/software/gnash/manual/doxygen/namespacegnash_1_1rtmp.html
	RTMP_EVENT_REQUEST_VERIFY		= 0x1a,
	RTMP_EVENT_RESPOND_VERIFY		= 0x1b,
	RTMP_EVENT_BUFFER_EMPTY			= 0x1f,
	RTMP_EVENT_BUFFER_READY			= 0x20,
};

int rtmp_set_chunk_size(bs_t *b, int chunk_size);
int rtmp_set_abort(bs_t *b, uint32_t chunk_streamid);
int rtmp_set_acknowledgement(bs_t *b, uint32_t sequence_number);
int rtmp_window_acknowledgement_size(bs_t *b, uint32_t window_size);
int rtmp_set_peer_bandwidth(bs_t *b, uint32_t window_size, uint8_t limit_type);
int rtmp_set_stream_begin(bs_t *b, uint32_t streamId);
int rtmp_set_stream_is_record(bs_t *b, uint32_t streamId);

#endif
