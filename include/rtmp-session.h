#ifndef __RMTP_SESSION_H__
#define __RMTP_SESSION_H__
#include "net-common.h"
#include "net-buffer.h"
#include "net-tcp-server.h"
#include "rtmp-handshake.h"
#include "rtmp-chunk-header.h"
#include "rtmp-write-command.h"
#include "shm-cache.h"
#include "rtmp-gop-cache.h"

#define RTMP_OUTPUT_CHUNK_SIZE	4096

typedef struct
{
    uint32_t in_chunk_size;    // read from network
    uint32_t out_chunk_size;   // write to network
    uint32_t sequence_number;  // bytes read report
    uint32_t window_size;      // server bandwidth (2500000)
    uint32_t peer_bandwidth;   // client bandwidth
    uint32_t buffer_length_ms; // s -> c
    uint8_t  limit_type;       // client bandwidth limit
    uint8_t  receive_audio;    // client don't want receive audio
    uint8_t  receive_video;
} rtmp_config;

typedef struct 
{
    rtmp_chunk_header header;
    uint8_t *ployload;
    uint32_t length;
} rtmp_packet;

typedef struct 
{
    SOCKET fd;
    uint8_t *pool;
    int pool_size;
    bs_t *send_buffer;
    rtmp_handshake state;
    rtmp_connect conn;
    rtmp_packet *packet;
    buffer_ptr buffer;
    rtmp_config config;
    sche_ptr scher;
    rtmp_gop *gop;
} rtmp_session, *rtmp_ptr;

int rtmp_recv_msg(void *user);
void *rtmp_create_session(SOCKET fd, buffer_ptr buffer, sche_ptr scher, void *gop);
int rtmp_detele_session(void *user);

#endif // !__RMTP_SESSION_H__
