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
    int interval;
    long long base_time;
    void *service;
    void (*start_stream)(void *service, frame_package *frame);
    void (*stop_stream)(void *service);
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
void gop_stop_to_playlive(rtmp_gop *gop, playlive_info *client);

frame_package *new_frame_package(int type, int size, uint8_t *frame, int frame_size);
void frame_package_release(frame_package *frame);
void frame_package_count(frame_package *frame);

#endif