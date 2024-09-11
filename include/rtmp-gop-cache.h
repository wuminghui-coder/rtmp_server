#ifndef __RTMP_GOP_CACHE_H__
#define __RTMP_GOP_CACHE_H__
#include "net-common.h"
#include <stdatomic.h>

typedef struct 
{
    atomic_int counter;
    int type;
    int size;
    uint8_t frame[0];
} frame_package;

typedef struct
{
    int status;
    int type;
    void *stream;
    int times;
    int flags;
    long long gtimess;
    int (*start_stream)(void *stream, frame_package *frame);
    int (*stop_stream)(void *stream);
} playlive_info, *playlive_ptr;

typedef struct
{
    frame_package *frame;
    struct list_head list;
} gop_cache;

typedef struct
{
    playlive_info *client;
    struct list_head list;
} playlive_client;

typedef struct
{
    gop_cache       *frame_sequence;
    playlive_client *client_sequence;
    frame_package   *pps;
    frame_package   *sps;
} rtmp_gop;

rtmp_gop *new_gop_cache(void);
void gop_set_pps(rtmp_gop *gop, frame_package *pps);
void gop_set_sps(rtmp_gop *gop, frame_package *sps);
void gop_pull_frame_to_cache(rtmp_gop *gop, frame_package *frame);
void gop_start_to_playlive(rtmp_gop *gop, playlive_info *stream);

frame_package *new_frame_package(int type, int size, uint8_t *frame, int frame_size);

#endif