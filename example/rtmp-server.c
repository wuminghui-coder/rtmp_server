#include "net-common.h"
#include "net-tcp-server.h"
#include "rtmp-session.h"
#include "rtmp-h264-packet.h"
#include "net-exception.h"

volatile int exit_flags = 1;

void signal_handler(int signl) 
{
    ERR("------recv sys signal = %d", signl);
    switch (signl) {
        case SIGINT:
        case SIGQUIT:
        case SIGSEGV:
        case SIGTERM:
            exit_flags = 0;
        default:
            return;
    }
}

int main()
{
    net_exception(signal_handler); 
    
    h264_stream *stream = h264_stream_init("/home/wmh/wmh/rtmp/example/files/test.h264");
    if (stream == NULL)
        return EXIT_FAILURE;

    shm_cache_ptr ring_cache = shm_cache_init(4096 * 1000);
    if (ring_cache == NULL)
    {
        ERR("create error");
        return EXIT_FAILURE;
    }

    server_ptr rtmp_server = tcp_start_server(DEFAULT_IP, 1935, 
                rtmp_create_session, rtmp_detele_session, rtmp_push_stream_session, rtmp_recv_msg);
    if (rtmp_server == NULL)
    {
        ERR("create rtmp server error");
        return EXIT_FAILURE;
    }

    h264_stream_start(stream, ring_cache, rtmp_server);

    while (exit_flags)
        sleep(1);

    h264_stream_unint(stream);

    shm_cache_unint(ring_cache);
    
    tcp_stop_server(rtmp_server);

    return EXIT_SUCCESS;
}
