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

    // file_packet * file = (file_packet *)calloc(1, sizeof(file_packet));

    // file->fp = fopen("/home/wmh/wmh/rtmp/example/files/test.h264", "rb+");
    // if (file->fp == NULL)
    //     return EXIT_FAILURE;

    // file->buffer = buffer_init(1024);
    // if (file->buffer == NULL)
    //     return EXIT_FAILURE;
        
    // sche_ptr scher = net_create_scheduler();
    // if (scher == NULL)
    // {
    //     ERR("create Scheduler");
    //     return EXIT_FAILURE;
    // }

    // timer_ptr timer_once = net_add_timer_task(scher, 0, 20, rtmp_read_h264_packet, (void *)file);

    // while (exit_flags)
    //     sleep(1);
    
    // net_delete_timer_task(timer_once);
    // net_destroy_scheduler(scher);
    
    SOCKET fd = CreateTcpSocket();
    Connect(fd, "127.0.0.1", 1934, 10);

    sche_ptr scher = net_create_scheduler();
    if (scher == NULL)
    {
        ERR("create Scheduler");
        return EXIT_FAILURE;
    }

    //ev_ptr ev = net_create_reader(scher, fd, tcp_new_connection, (void *)server);

    while (exit_flags)
        sleep(1);
        
    net_destroy_scheduler(scher);

    return EXIT_SUCCESS;
}

