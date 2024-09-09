#ifndef  __RTMP_WRITE_PACKET_H__
#define  __RTMP_WRITE_PACKET_H__
#include "rtmp-session.h"
#include "net-common.h"

int rtmp_write_packet(rtmp_ptr rtmp, rtmp_chunk_header *header, uint8_t *payload);

#endif // __RTMP_WRITE_PACKET_H__