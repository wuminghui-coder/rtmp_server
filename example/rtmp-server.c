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
    //net_exception(signal_handler); 

    gop_cache *gop = create_gop_cache();
    if (gop == NULL)
        return EXIT_FAILURE;

    h264_stream *stream = h264_stream_init("/home/wmh/wmh/rtmp/example/files/test.h264", gop);
    if (stream == NULL)
        return EXIT_FAILURE;

    server_ptr rtmp_server = tcp_start_server(DEFAULT_IP, 1935, 
                rtmp_create_session, rtmp_detele_session, rtmp_recv_msg, gop);
    if (rtmp_server == NULL)
    {
        ERR("create rtmp server error");
        return EXIT_FAILURE;
    }

    sche_ptr scher = net_create_scheduler();
    if (scher == NULL)
        return EXIT_FAILURE;

    timer_ptr pull_stream = net_add_timer_task(scher, 0, 30, rtmp_pull_h264_stream, (void *)stream);

    while (exit_flags)
        sleep(1);

    h264_stream_unint(stream);

    tcp_stop_server(rtmp_server);

    return EXIT_SUCCESS;
}
