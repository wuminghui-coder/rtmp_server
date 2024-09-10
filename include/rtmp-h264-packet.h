#ifndef __RMTP_H264_H__
#define __RMTP_H264_H__
#include "net-common.h"
#include "net-buffer.h"
#include "rtmp-session.h"
#include "shm-cache.h"
#include "net-schedule.h"
#include "net-tcp-server.h"
#include "rtmp-gop-cache.h"

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
    gop_cache *gop;
    shm_cache_ptr ring_cache;
    sche_ptr scher;
    timer_ptr pull_stream;
    timer_ptr push_stream;
} h264_stream;

void h264_stream_unint(h264_stream *stream);
h264_stream *h264_stream_init(const char *file, gop_cache *gop);
int rtmp_pull_h264_stream(void *args);
frame_info *rtmp_write_frame(frame_info *frame);
frame_info *rtmp_write_avc_sequence(frame_info *sps, frame_info *pps);
void h264_start_stream(h264_stream * stream);

#endif // !__RMTP_H264_H__


