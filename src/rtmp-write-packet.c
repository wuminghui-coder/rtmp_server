#include "rtmp-write-packet.h"

int rtmp_write_packet(rtmp_ptr rtmp, rtmp_chunk_header *header, uint8_t *payload)
{
    if (rtmp == NULL || header == NULL || payload == NULL)
        return NET_SUCCESS;

    bs_reset(rtmp->send_buffer);
    int payload_size = 0, headerSize = 0, chunk_size = 0;
    payload_size = header->length;

    rtmp_chunk_write_header_type(rtmp->send_buffer, header);
    if (header->timestamp >= 0xFFFFFF)
       rtmp_chunk_write_extended_timestamp(rtmp->send_buffer, header->timestamp);
    int index = 0;
    while (payload_size > 0)
    {
        chunk_size = payload_size < rtmp->config.out_chunk_size ? payload_size : rtmp->config.out_chunk_size;
        bs_write_bytes(rtmp->send_buffer, payload + index, chunk_size);
        int code = send(rtmp->fd, bs_start_ptr(rtmp->send_buffer), bs_pos(rtmp->send_buffer), 0);
        if (code <= 0)
            break;

        payload_size -= chunk_size;
        index += chunk_size;
        if (payload_size > 0)
        {
            bs_reset(rtmp->send_buffer);
            bs_write_u(rtmp->send_buffer, 2, RTMP_CHUNK_TYPE_3);
            bs_write_u(rtmp->send_buffer, 6, header->csid);
        }
    }
    return NET_SUCCESS;
}
