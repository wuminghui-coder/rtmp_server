#include "rtmp-gop-cache.h"
#include "rtmp-h264-packet.h"
#include "net-task.h"

gop_cache *create_gop_cache(void)
{
    gop_cache *gop = (gop_cache *)calloc(1, sizeof(gop_cache));
    if (!gop)
        return NULL;
    
    gop->frame_sequence = (gop_list *)calloc(1, sizeof(gop_list));
    if (!gop->frame_sequence)
        return NULL;

    gop->stream_sequence = (stream_list *)calloc(1, sizeof(stream_list));
    if (!gop->stream_sequence)
        return NULL;

    INIT_LIST_HEAD(&gop->frame_sequence->list);
    INIT_LIST_HEAD(&gop->stream_sequence->list);

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

static void push_frame_to_stream(gop_cache *gop, frame_info *frame)
{
    stream_list *task_node = NULL;
    stream_list *temp_node = NULL;
    list_for_each_entry_safe(task_node, temp_node, &gop->stream_sequence->list, list)
    {
        stream_ptr conn = (stream_ptr)task_node->stream;
        if (conn == NULL)
            continue;

        if (conn->start_stream)
            conn->start_stream(gop, frame);
    }
}

static void push_cache_to_playlive(gop_cache *gop, stream_info *stream)
{
    gop_list *task_node = NULL;
    gop_list *temp_node = NULL;
    list_for_each_entry_safe(task_node, temp_node, &gop->frame_sequence->list, list)
    {
        frame_info *frame = (frame_info *)task_node->frame;
        if (frame == NULL)
            continue;

        frame_info *f = NULL;
        if (stream->type == RTMP_SERVER)
            f = rtmp_write_frame(frame);

        if (stream->start_stream)
            stream->start_stream(gop, f);
    }
}

static void push_frame_to_gop(gop_cache *gop, frame_info *frame)
{
    gop_list *new_task = (gop_list *)calloc(1, sizeof(gop_list));
    if (!new_task)
        return;

    new_task->frame = frame;

    list_add_tail(&new_task->list, &gop->frame_sequence->list);
}

static void gop_reset_cache(gop_cache *gop)
{
    gop_list *task_node = NULL;
    gop_list *temp_node = NULL;
    list_for_each_entry_safe(task_node, temp_node, &gop->frame_sequence->list, list)
    {
        list_del(&task_node->list);
        if ((frame_info *)task_node->frame) 
            net_free(task_node->frame->frame)                     
            net_free(task_node->frame)                          
        net_free(task_node)         
    }
}

void gop_pull_frame_to_cache(gop_cache *gop, frame_info *frame)
{
    if (!gop || !frame)
        return;

    push_frame_to_stream(gop, frame);

    if (frame->type == NAL_UNIT_TYPE_CODED_SLICE_IDR)
    {
        DBG("reset gop");
        gop_reset_cache(gop);
    }

    push_frame_to_gop(gop, frame);
}

static void register_stream(gop_cache *gop, stream_info *stream)
{
    stream_list *new_task = (stream_list *)calloc(1, sizeof(stream_list));
    if (!new_task)
        return;

    new_task->stream = stream;

    list_add_tail(&new_task->list, &gop->stream_sequence->list);
}

static void unregister_stream(gop_cache *gop, stream_info *stream)
{
    stream_list *task_node = NULL;
    stream_list *temp_node = NULL;
    list_for_each_entry_safe(task_node, temp_node, &gop->stream_sequence->list, list)
    {
        if (task_node && task_node->stream == stream) 
        {
            list_del(&task_node->list);
            if ((stream_info *)task_node->stream)                      
                net_free(task_node->stream)                          
            net_free(task_node)   
        }               
    }                                                
}

void gop_start_to_playlive(gop_cache *gop, stream_info *stream)
{
    if (!gop || !stream)
        return;

    push_cache_to_playlive(gop, stream);

    register_stream(gop, stream);
}

frame_info *new_frame(int type, int pos, uint8_t *frame, int size)
{
    frame_info *f = (frame_info *)calloc(1, sizeof(frame_info));
    if (f == NULL)
        return NULL;

    f->frame = (uint8_t *)calloc(1, size);
    if (f->frame == NULL)
    {
        net_free(f);
        return NULL;
    }

    memcpy(f->frame, frame, size);

    f->size  = size;
    f->type  = type;
    return f;
}
