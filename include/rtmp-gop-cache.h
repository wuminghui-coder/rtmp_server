#ifndef __RTMP_GOP_CACHE_H__
#define __RTMP_GOP_CACHE_H__
#include "net-common.h"

typedef struct 
{
    int type;
    int size;
    uint8_t *frame;
} frame_info;

typedef struct
{
    void *frame_data;
    struct list_head list;
} gop_list;

typedef struct
{
    gop_list *frame_sequence;
    frame_info *pps;
    frame_info *sps;
} gop_cache;


gop_cache *create_gop_cache(void);
void gop_set_pps(gop_cache *gop, frame_info *pps);
void gop_set_sps(gop_cache *gop, frame_info *sps);

#endif