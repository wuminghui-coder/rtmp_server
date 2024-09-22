#ifndef __RMTP_SNED_REPLY_H__
#define __RMTP_SNED_REPLY_H__
#include "rtmp-session.h"
#include "net-common.h"

#define RTMP_FMSVER				"FMS/3,0,1,123"
#define RTMP_CAPABILITIES		31

#define RTMP_STREAM_LIVE	"live"
#define RTMP_STREAM_RECORD	"record"
#define RTMP_STREAM_APPEND	"append"

#define RTMP_LEVEL_WARNING	"warning"
#define RTMP_LEVEL_STATUS	"status"
#define RTMP_LEVEL_ERROR	"error"
#define RTMP_LEVEL_FINISH	"finish" // ksyun cdn
#define RTMP_WINDOW_SIZE    5000000

int rtmp_reply_connect(rtmp_ptr rtmp, int code, double transactionId);
int rtmp_reply_result(rtmp_ptr rtmp, int code, double transactionId);
int rtmp_reply_onplay(rtmp_ptr rtmp, int code, double transactionId);
int rtmp_reply_onstatus(rtmp_ptr rtmp, int code, double transactionId);

#endif // !__RMTP_SNED_REPLY_H__
