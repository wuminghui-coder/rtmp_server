#include "rtmp-gop-cache.h"
#include "rtmp-h264-packet.h"
#include "net-task.h"
#include "rtmp-push-stream.h"

rtmp_gop *new_gop_cache(void)
{
    rtmp_gop *gop = (rtmp_gop *)calloc(1, sizeof(rtmp_gop));
    if (!gop)
        return NULL;
    
    gop->frame_sequence = (gop_cache *)calloc(1, sizeof(gop_cache));
    if (!gop->frame_sequence)
        return NULL;

    gop->client_sequence = (playlive_client *)calloc(1, sizeof(playlive_client));
    if (!gop->client_sequence)
        return NULL;

    INIT_LIST_HEAD(&gop->frame_sequence->list);
    INIT_LIST_HEAD(&gop->client_sequence->list);

    gop->pps = NULL;
    gop->sps = NULL;

    return gop;
}

void gop_set_pps(rtmp_gop *gop, frame_package *pps)
{
    if (!gop || !pps)
        return;

    if (gop->pps)
        net_free(gop->pps);
    gop->pps = pps;
}

void gop_set_sps(rtmp_gop *gop, frame_package *sps)
{
    if (!gop || !sps)
        return;

    if (gop->sps)
        net_free(gop->sps);

    gop->sps = sps;
}

static void _push_frame_to_playlive(rtmp_gop *gop, frame_package *frame)
{
    if (!gop || !frame)
        return;

    frame_package *f = rtmp_write_frame(frame);
    if (f == NULL)
        return;

    playlive_client *client_node = NULL;
    playlive_client *temp_node = NULL;
    list_for_each_entry_safe(client_node, temp_node, &gop->client_sequence->list, list)
    {
        playlive_ptr client = (playlive_ptr)client_node->client;
        if (client == NULL)
            continue;

        if (client->start_stream)
            client->start_stream(client, f);
    }
}

static void push_cache_to_playlive(rtmp_gop *gop, playlive_info *stream)
{
    gop_cache *client_node = NULL;
    gop_cache *temp_node = NULL;

    frame_package *sp = rtmp_write_avc_sequence(gop->sps, gop->pps);
    if (stream->start_stream)
        stream->start_stream(stream, sp);

    list_for_each_entry_safe(client_node, temp_node, &gop->frame_sequence->list, list)
    {
        frame_package *frame = (frame_package *)client_node->frame;
        if (frame == NULL)
            continue;

        frame_package *f = rtmp_write_frame(frame);

        if (stream->start_stream)
            stream->start_stream(stream, f);
    }
}

static void push_frame_to_gop(rtmp_gop *gop, frame_package *frame)
{
    gop_cache *new_task = (gop_cache *)calloc(1, sizeof(gop_cache));
    if (!new_task)
        return;

    new_task->frame = frame;

    list_add_tail(&new_task->list, &gop->frame_sequence->list);
}

static void gop_reset_cache(rtmp_gop *gop)
{
    gop_cache *client_node = NULL;
    gop_cache *temp_node = NULL;
    list_for_each_entry_safe(client_node, temp_node, &gop->frame_sequence->list, list)
    {
        list_del(&client_node->list);
        if ((frame_package *)client_node->frame) 
            //net_free(client_node->frame->frame)                     
            net_free(client_node->frame)                          
        net_free(client_node)         
    }
}

void gop_pull_frame_to_cache(rtmp_gop *gop, frame_package *frame)
{
    if (!gop || !frame)
        return;

    _push_frame_to_playlive(gop, frame);

    if (frame->type == NAL_UNIT_TYPE_CODED_SLICE_IDR)
    {
        DBG("reset gop");
        gop_reset_cache(gop);
    }

    push_frame_to_gop(gop, frame);
}

static void register_stream(rtmp_gop *gop, playlive_info *stream)
{
    playlive_client *new_task = (playlive_client *)calloc(1, sizeof(playlive_client));
    if (!new_task)
        return;

    new_task->client = stream;

    list_add_tail(&new_task->list, &gop->client_sequence->list);
}

static void unregister_stream(rtmp_gop *gop, playlive_info *stream)
{
    playlive_client *client_node = NULL;
    playlive_client *temp_node = NULL;
    list_for_each_entry_safe(client_node, temp_node, &gop->client_sequence->list, list)
    {
        if (client_node && client_node->client == stream) 
        {
            list_del(&client_node->list);
            if ((playlive_info *)client_node->client)                      
                net_free(client_node->client)                          
            net_free(client_node)   
        }               
    }                                                
}

void gop_start_to_playlive(rtmp_gop *gop, playlive_info *stream)
{
    if (!gop || !stream)
        return;

    push_cache_to_playlive(gop, stream);

    register_stream(gop, stream);
}

frame_package *new_frame_package(int type, int size, uint8_t *frame, int frame_size)
{
    frame_package *f = (frame_package *)calloc(1, sizeof(frame_package) + size);
    if (f == NULL)
        return NULL;

    if (frame && frame_size <= size)
        memcpy(f->frame, frame, frame_size);

    f->size  = size;
    f->type  = type;
    f->counter = ATOMIC_VAR_INIT(0);
    
    return f;
}
