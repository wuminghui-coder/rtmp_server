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

    pthread_mutex_init(&gop->lock, NULL);
    //pthread_mutex_destroy(&lock);
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
    pthread_mutex_lock(&gop->lock);

    if (gop->pps)
        net_free(gop->pps);
    gop->pps = pps;

    pthread_mutex_unlock(&gop->lock); 

}

void gop_set_sps(rtmp_gop *gop, frame_package *sps)
{
    if (!gop || !sps)
        return;
    pthread_mutex_lock(&gop->lock);

    if (gop->sps)
        net_free(gop->sps);

    gop->sps = sps;

    pthread_mutex_unlock(&gop->lock); 

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

static void _push_cache_to_playlive(rtmp_gop *gop, playlive_info *client)
{
    if (!gop || !client)
        return;
    
    gop_cache *client_node = NULL;
    gop_cache *temp_node = NULL;

    frame_package *avc = rtmp_write_avc_sequence(gop->sps, gop->pps);
    if (client->start_stream)
        client->start_stream(client, avc);

    list_for_each_entry_safe(client_node, temp_node, &gop->frame_sequence->list, list)
    {
        frame_package *frame = (frame_package *)client_node->frame;
        if (frame == NULL)
            continue;

        frame_package *f = rtmp_write_frame(frame);
        if (f == NULL)
            continue;

        if (client->start_stream)
            client->start_stream(client, f);
    }
}

static void _push_frame_to_cache(rtmp_gop *gop, frame_package *frame)
{
    if (!gop || !frame)
        return;

    gop_cache *new_task = (gop_cache *)calloc(1, sizeof(gop_cache));
    if (!new_task)
        return;

    new_task->frame = frame;

    list_add_tail(&new_task->list, &gop->frame_sequence->list);
}

static void _gop_reset_cache(rtmp_gop *gop)
{
    if (!gop)
        return;

    gop_cache *client_node = NULL;
    gop_cache *temp_node   = NULL;
    list_for_each_entry_safe(client_node, temp_node, &gop->frame_sequence->list, list)
    {
        list_del(&client_node->list);                  
        net_free(client_node->frame);                          
        net_free(client_node);         
    }
}

void gop_pull_frame_to_cache(rtmp_gop *gop, frame_package *frame)
{
    if (!gop || !frame)
        return;

    pthread_mutex_lock(&gop->lock);

    _push_frame_to_playlive(gop, frame);

    if (frame->type == NAL_UNIT_TYPE_CODED_SLICE_IDR)
        _gop_reset_cache(gop);
    
    _push_frame_to_cache(gop, frame);

    pthread_mutex_unlock(&gop->lock); 
}

static void _register_client(rtmp_gop *gop, playlive_info *client)
{
    if (!gop || !client)
        return;

    playlive_client *new_task = (playlive_client *)calloc(1, sizeof(playlive_client));
    if (!new_task)
        return;

    new_task->client = client;

    list_add_tail(&new_task->list, &gop->client_sequence->list);
}

static void _unregister_client(rtmp_gop *gop, playlive_info *client)
{
    playlive_client *client_node = NULL;
    playlive_client *temp_node = NULL;
    list_for_each_entry_safe(client_node, temp_node, &gop->client_sequence->list, list)
    {
        if (client_node && client_node->client == client)
        {
            list_del(&client_node->list);                    
            net_free(client_node->client)                          
            net_free(client_node)   
        }               
    }                                                
}

void gop_start_to_playlive(rtmp_gop *gop, playlive_info *client)
{
    if (!gop || !client)
        return;

    pthread_mutex_lock(&gop->lock);

    _push_cache_to_playlive(gop, client);

    _register_client(gop, client);

    pthread_mutex_unlock(&gop->lock); 
}

void gop_stop_to_playlive(rtmp_gop *gop, playlive_info *client)
{
    if (!gop || !client)
        return;

    pthread_mutex_lock(&gop->lock);

    _unregister_client(gop, client);

    pthread_mutex_unlock(&gop->lock); 
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

// atomic_fetch_sub(&counter, 1);
// int old_value = atomic_exchange(&counter, 5);
//  int expected = 5;
//  int desired = 10;
//  bool success = atomic_compare_exchange_strong(&counter, &expected, desired);
//  // 如果 counter 的值等于 expected，则将其设置为 desired 并返回 true；否则，更新 expected 为 counter 的当前值并返回 false
//  int value = atomic_load(&counter); // 读取 counter 的值
//  atomic_store(&counter, 20); // 将 counter 的值设置为 20
void frame_package_release(frame_package *frame)
{
    if (!frame)
        return;

    atomic_fetch_sub(&frame->counter, 1);
    if (atomic_load(&frame->counter) == 0)
        net_free(frame);
}

void frame_package_count(frame_package *frame)
{
    if (!frame)
        return;
        
    atomic_fetch_add(&frame->counter, 1);
}

playlive_ptr new_playlive(void *server, stream_function start_stream)
{
    if (!server || !start_stream)
        return NULL;
        
    playlive_ptr client = (playlive_ptr)calloc(1, sizeof(playlive_info));
    if (!client)
        return NULL;  

    client->base_time     = 1000; 
    client->interval      = 40;
    client->interval_test = get_time_ms(); 
    client->service       = server;
    client->start_stream  = start_stream;

    return client;
}