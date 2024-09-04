#include "rtmp-gop-cache.h"

gop_cache *create_gop_cache(void)
{
    gop_cache *gop = (gop_cache *)calloc(1, sizeof(gop_cache));
    if (!gop)
        return NULL;
    
    gop->frame_sequence = (gop_list *)calloc(1, sizeof(gop_list));
    if (!gop->frame_sequence)
        return NULL;

    INIT_LIST_HEAD(&gop->frame_sequence->list);


    gop->pps = NULL;
    gop->sps = NULL;

    return gop;
}

void gop_set_pps(gop_cache *gop, frame_info *pps)
{
    if (!gop || !pps)
        return;

    if (gop->pps)
        net_free(gop->pps);

    gop->pps = pps;
}

void gop_set_sps(gop_cache *gop, frame_info *sps)
{
    if (!gop || !sps)
        return;

    if (gop->pps)
        net_free(gop->pps);

    gop->pps = sps;
}











