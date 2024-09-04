#include "rtmp-session.h"
#include "rtmp-send-message.h"
#include "rtmp-recv-event.h"
#include "rtmp-push-stream.h"

int rtmp_parse_packet(rtmp_ptr rtmp)
{
    if (rtmp == NULL || rtmp->buffer == NULL)
        return NET_FAIL;

    bs_t *b = bs_new(begin_read(rtmp->buffer), readable_bytes(rtmp->buffer));
    if (b == NULL)
        return NET_FAIL; 
    
    if (!rtmp->packet)
    {
        rtmp_chunk_header header = {0};
        rtmp_chunk_read_basic_header(b, &header);
        rtmp_chunk_read_message_header(b, &header);
        rtmp_chunk_show(&header);

        rtmp->packet = (rtmp_packet *)calloc(1, sizeof(rtmp_packet));
        if (rtmp->packet == NULL)
            return NET_FAIL;
        
        memcpy(&rtmp->packet->header, &header, sizeof(rtmp_chunk_header));   
        rtmp->packet->length = rtmp->packet->header.length > bs_bytes_left(b) ? bs_bytes_left(b) : rtmp->packet->header.length;
        if(rtmp->packet->length > 0)
        {
            rtmp->packet->ployload = (uint8_t *)calloc(1, rtmp->packet->length);
            memcpy(rtmp->packet->ployload, bs_p_ptr(b), rtmp->packet->length); 
            bs_skip_bytes(b, rtmp->packet->length);
        }

        //LOG("buffer size %d, pos %d, left %d, header length %d, buffer length %d", readable_bytes(rtmp->buffer), bs_pos(b),  
        //        bs_bytes_left(b), rtmp->packet->header.length, rtmp->packet->length);
    } else {
        int residue = rtmp->packet->header.length -  rtmp->packet->length <= bs_bytes_left(b) ? rtmp->packet->header.length -  rtmp->packet->length : bs_bytes_left(b);
        uint8_t *new_load = (uint8_t *)realloc(rtmp->packet->ployload, rtmp->packet->length + residue);
        if (new_load == NULL)
            return NET_FAIL;

        rtmp->packet->ployload = new_load;

        memcpy(rtmp->packet->ployload + rtmp->packet->length, bs_p_ptr(b), residue);
        
        rtmp->packet->length += residue;
        
        bs_skip_bytes(b, residue);
    }

    retrieve(rtmp->buffer, bs_pos(b));
    bs_free(b);
    if (rtmp->packet->length == rtmp->packet->header.length)
        return NET_SUCCESS;
    return NET_FAIL;      
}

int rtmp_recv_msg(void *user)
{
    if (user == NULL)
        return NET_FAIL;

    rtmp_ptr rtmp = (rtmp_ptr)user;
    LOG("");
    LOG("-----------------------------------------")
    LOG("recv size:%d", readable_bytes(rtmp->buffer));

    while (readable_bytes(rtmp->buffer) > 0)
    {
        if (rtmp->state == RTMP_HANDSHAKE_UNINIT)
        {
            if (readable_bytes(rtmp->buffer) < RTMP_HANDSHAKE_SIZE + 1)
                break;
            rtmp_server_send_handshake(rtmp);
            retrieve(rtmp->buffer, RTMP_HANDSHAKE_SIZE + 1);
            rtmp->state = RTMP_HANDSHAKE_0;
        } else if (rtmp->state == RTMP_HANDSHAKE_0)
        {
            if (readable_bytes(rtmp->buffer) < RTMP_HANDSHAKE_SIZE)
                break;
            retrieve(rtmp->buffer, RTMP_HANDSHAKE_SIZE);
            rtmp->state = RTMP_HANDSHAKE_1;
        } else {
            if (!rtmp_parse_packet(rtmp))
                rtmp_recv_event(rtmp);
            else 
                break;
        }
    }

    return NET_SUCCESS;
}

void *rtmp_create_session(SOCKET fd, buffer_ptr buffer, sche_ptr scher)
{   
    if (buffer == NULL)
        return NULL;

    rtmp_ptr rtmp = (rtmp_ptr)calloc(1, sizeof(rtmp_session));
    if (rtmp == NULL)
        return NULL;

    rtmp->pool_size = RTMP_HANDSHAKE_SIZE * 3;

    rtmp->pool = (uint8_t *)calloc(1, rtmp->pool_size);
    if (rtmp->pool == NULL)
    {
        net_free(rtmp);
        return NULL;
    }

    rtmp->send_buffer = bs_new(rtmp->pool, rtmp->pool_size);

    rtmp->state = RTMP_HANDSHAKE_UNINIT;
    rtmp->fd = fd;
    rtmp->buffer = buffer;
    rtmp->packet = NULL;
    rtmp->config.out_chunk_size = RTMP_OUTPUT_CHUNK_SIZE;

    rtmp->cache = shm_cache_init(4096 * 1000);
    rtmp->scher = scher;
    
    return rtmp;
}

int rtmp_push_stream_session(void *user, uint8_t *data, int size, int type)
{
    if (user == NULL || data == NULL)
        return NET_FAIL;

    rtmp_ptr rtmp = (rtmp_ptr)user;

    if (rtmp->cache)
        shm_cache_put(rtmp->cache, data, size, type);
    return NET_SUCCESS;
}

int rtmp_detele_session(void *user)
{
    if (user == NULL)
        return NET_FAIL;

    rtmp_ptr rtmp = (rtmp_ptr)user;
    if (rtmp->pool)
        net_free(rtmp->pool);

    if (rtmp->pull_stream)
        rtmp_stop_push_stream(rtmp);

    if (rtmp->send_buffer)
        bs_free(rtmp->send_buffer);
    
    if (rtmp->cache)
        shm_cache_unint(rtmp->cache);

    if (rtmp)
        net_free(rtmp);

    return NET_SUCCESS;
}
