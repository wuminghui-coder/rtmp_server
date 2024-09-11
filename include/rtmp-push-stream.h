#ifndef __RTMP_PUSH_STREAM_H__
#define __RTMP_PUSH_STREAM_H__
#include "net-common.h"
#include "rtmp-session.h"

#define RTMP_FRAME_HEADER_LENGTH 9
#define RTMP_AVC_HEADER_LENGTH   16

int rtmp_start_push_stream(rtmp_ptr rtmp);
frame_package *rtmp_write_frame(frame_package *frame);
frame_package *rtmp_write_avc_sequence(frame_package *sps, frame_package *pps);

#endif // !__RTMP_PUSH_STREAM_H__