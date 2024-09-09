#ifndef __RTMP_GOP_CACHE_H__
#define __RTMP_GOP_CACHE_H__
#include "net-common.h"

#define RTMP_SERVER 1

typedef struct 
{
    int type;
    int size;
    uint8_t *frame;
} frame_info;

typedef struct
{
    int status;
    int type;
    void *stream;
    int (*start_stream)(void *stream, frame_info *frame);
    int (*stop_stream)(void *stream);
} stream_info, *stream_ptr;

typedef struct
{
    frame_info *frame;
    struct list_head list;
} gop_list;

typedef struct
{
    stream_info *stream;
    struct list_head list;
} stream_list;

typedef struct
{
    gop_list *frame_sequence;
    stream_list *stream_sequence;
    frame_info *pps;
    frame_info *sps;
} gop_cache;

gop_cache *create_gop_cache(void);
void gop_set_pps(gop_cache *gop, frame_info *pps);
void gop_set_sps(gop_cache *gop, frame_info *sps);
void gop_pull_frame_to_cache(gop_cache *gop, frame_info *frame);
void gop_start_to_playlive(gop_cache *gop, stream_info *stream);
frame_info *new_frame(int type, int pos, uint8_t *frame, int size);

#endif