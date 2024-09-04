#ifndef __RTMP_INVOKE_HANDLE__
#define __RTMP_INVOKE_HANDLE__
#include "net-common.h"
#include "rtmp-session.h"

enum {
    StreamBegin,
    StreamEOF,
    StreamDry,
    SetBufferLength,
    StreamIsRecorded,
    PingRequest,
    PingResponse,
};

typedef struct {
    const char *command;
    int (*function)(bs_t *b, rtmp_ptr rtmp);
    int (*reply)(rtmp_ptr rtmp, int code, double transactionId);
} rtmp_command_handle, *rtmp_command_ptr;

int rtmp_recv_paser_invoke(rtmp_session *rtmp, bs_t *b);

#endif // !__RTMP_INVOKE_HANDLE__

