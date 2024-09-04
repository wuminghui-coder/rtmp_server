#ifndef __RMTP_H264_H__
#define __RMTP_H264_H__
#include "net-common.h"
#include "net-buffer.h"
#include "rtmp-session.h"
#include "shm-cache.h"
#include "net-schedule.h"
#include "net-tcp-server.h"

enum {
    NAL_UNIT_TYPE_UNSPECIFIED = 0,                    // Unspecified
    NAL_UNIT_TYPE_CODED_SLICE_NON_IDR,                // Coded slice of a non-IDR picture
    NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_A,       // Coded slice data partition A
    NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_B,       // Coded slice data partition B
    NAL_UNIT_TYPE_CODED_SLICE_DATA_PARTITION_C,       // Coded slice data partition C
    NAL_UNIT_TYPE_CODED_SLICE_IDR = 5,                // Coded slice of an IDR picture
    NAL_UNIT_TYPE_SEI,                                // Supplemental enhancement information (SEI)
    NAL_UNIT_TYPE_SPS = 7,                            // Sequence parameter set
    NAL_UNIT_TYPE_PPS = 8,                            // Picture parameter set
    NAL_UNIT_TYPE_AUD,                                // Access unit delimiter
    NAL_UNIT_TYPE_END_OF_SEQUENCE,                    // End of sequence
    NAL_UNIT_TYPE_END_OF_STREAM,                      // End of stream
    NAL_UNIT_TYPE_FILLER,                             // Filler data
    NAL_UNIT_TYPE_SPS_EXT,                            // Sequence parameter set extension
    NAL_UNIT_TYPE_PREFIX_NAL,                         // Prefix NAL unit
    NAL_UNIT_TYPE_SUBSET_SPS,                         // Subset Sequence parameter set
    NAL_UNIT_TYPE_DPS,                                // Depth Parameter Set
                                                      // 17..18    // Reserved
    NAL_UNIT_TYPE_CODED_SLICE_AUX = 19,               // Coded slice of an auxiliary coded picture without partitioning
    NAL_UNIT_TYPE_CODED_SLICE_SVC_EXTENSION,          // Coded slice of SVC extension
                                                      // 20..23    // Reserved
                                                      // 24..31    // Unspecified
};

typedef struct 
{
    FILE *fp;
    buffer_ptr buffer;
    cache_buffer *stream_info[2];
    shm_cache_ptr cache;
    sche_ptr scher;
    timer_ptr pull_stream;
    timer_ptr push_stream;
    server_ptr rtmp_server;
} h264_stream;

h264_stream *h264_stream_init(const char *file);
void h264_stream_unint(h264_stream *stream);
int h264_stream_start(h264_stream *stream, shm_cache_ptr cache, server_ptr rtmp_server);

#endif // !__RMTP_H264_H__
