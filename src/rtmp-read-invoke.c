#include "rtmp-read-invoke.h"
#include "rtmp-send-message.h"
#include "amf0.h"
#include "rtmp-write-command.h"
#include "rtmp-send-reply.h"

#define N_STREAM_NAME	256

static int rtmp_read_onconnect(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL ||rtmp == NULL)
        return NET_FAIL;

    retrieve(rtmp->buffer, readable_bytes(rtmp->buffer));
    
    rtmp_connect *conn = &rtmp->conn;

	amf_object_item items[1];
	amf_object_item commands[9];

	conn->encoding = (double)RTMP_ENCODING_AMF_0;

    AMF_OBJECT_ITEM_VALUE(commands[0], AMF_STRING, "app", conn->app, sizeof(conn->app));
    AMF_OBJECT_ITEM_VALUE(commands[1], AMF_STRING, "flashVer", conn->flashver, sizeof(conn->flashver));
    AMF_OBJECT_ITEM_VALUE(commands[2], AMF_STRING, "tcUrl", conn->tcUrl, sizeof(conn->tcUrl));
    AMF_OBJECT_ITEM_VALUE(commands[3], AMF_BOOLEAN, "fpad", &conn->fpad, 1);
    AMF_OBJECT_ITEM_VALUE(commands[4], AMF_NUMBER, "capabilities", &conn->capabilities, 8);
	AMF_OBJECT_ITEM_VALUE(commands[5], AMF_NUMBER, "audioCodecs", &conn->audioCodecs, 8);
 	AMF_OBJECT_ITEM_VALUE(commands[6], AMF_NUMBER, "videoCodecs", &conn->videoCodecs, 8);
 	AMF_OBJECT_ITEM_VALUE(commands[7], AMF_NUMBER, "videoFunction", &conn->videoFunction, 8);
 	AMF_OBJECT_ITEM_VALUE(commands[8], AMF_NUMBER, "objectEncoding", &conn->encoding, 8);
	AMF_OBJECT_ITEM_VALUE(items[0],    AMF_OBJECT, "command", commands, sizeof(commands) / sizeof(commands[0]));
    
    return amf_read_object_item(b, bs_read_u8(b), items);
}

static int rtmp_read_oncreate_stream(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

    amf_object_item items[1];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_NULL, "command", NULL, 0);
    return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onplay(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

    char string[24] = {0};
    double number = 0;
	amf_object_item items[3];

    AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
 	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "stream", string, sizeof(string));
 	AMF_OBJECT_ITEM_VALUE(items[2], AMF_NUMBER, "stream", &number, 8);
    return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onget_stream_length(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

    char string[24] = {0};
	amf_object_item items[2];

    AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
 	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "stream", string, sizeof(string));
    return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_ondelete_stream(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

    double stream_id = 0;
    amf_object_item items[2];

    AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_NUMBER, "streamId", &stream_id, 8);
    return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onreceive_audio(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

    uint8_t receiveAudio = 1;
    amf_object_item items[2];

    AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_BOOLEAN, "receiveAudio", &receiveAudio, 1);
    return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onreceive_video(bs_t *b, rtmp_ptr rtmp)
{
	if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

	uint8_t receiveVideo = 1;
	amf_object_item items[2];

	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_BOOLEAN, "receiveVideo", &receiveVideo, 1);

	return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onpublish(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

	char stream_name[N_STREAM_NAME] = { 0 };
	char stream_type[18] = { 0 }; 

	amf_object_item items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "name", stream_name, sizeof(stream_name));
	AMF_OBJECT_ITEM_VALUE(items[2], AMF_STRING, "type", stream_type, sizeof(stream_type));

	return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onseek(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

	double milliSeconds = 0;

	amf_object_item items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_NUMBER, "milliSeconds", &milliSeconds, 8);

	return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onpause(bs_t *b, rtmp_ptr rtmp)
{
	if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

	uint8_t pause = 0; 
	double milliSeconds = 0;
	amf_object_item items[2];

	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_BOOLEAN, "pause", &pause, 1);
	AMF_OBJECT_ITEM_VALUE(items[2], AMF_NUMBER, "milliSeconds", &milliSeconds, 8);

	return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onfcpublish(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

	char stream_name[N_STREAM_NAME] = { 0 };
	amf_object_item items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "playpath", stream_name, sizeof(stream_name));

	return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onfcunpublish(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

	char stream_name[N_STREAM_NAME] = { 0 };
	amf_object_item items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "playpath", stream_name, sizeof(stream_name));

	return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onfcsubscribe(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

	char subscribe[N_STREAM_NAME] = { 0 };
	amf_object_item items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "subscribepath", subscribe, sizeof(subscribe));

	return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

static int rtmp_read_onfcunsubscribe(bs_t *b, rtmp_ptr rtmp)
{
    if (b == NULL || bs_bytes_left(b) < 1)
        return NET_FAIL;

	char subscribe[N_STREAM_NAME] = { 0 };
	amf_object_item items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", NULL, 0);
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "subscribepath", subscribe, sizeof(subscribe));

	return amf_read_item(b, items, sizeof(items) / sizeof(items[0]));
}

rtmp_command_handle g_command_handle[] = {
    { "connect",         rtmp_read_onconnect,              rtmp_reply_connect},
    { "createStream",    rtmp_read_oncreate_stream,        rtmp_reply_create_stream},
    { "getStreamLength", rtmp_read_onget_stream_length,    NULL},
    { "play",            rtmp_read_onplay,                 rtmp_reply_onplay},
    { "deleteStream",	 rtmp_read_ondelete_stream,        NULL},
	{ "receiveAudio",	 rtmp_read_onreceive_audio,        NULL},
	{ "receiveVideo",	 rtmp_read_onreceive_video,        NULL},
	{ "publish",		 rtmp_read_onpublish,              NULL},
	{ "seek",			 rtmp_read_onseek,                 NULL},
	{ "pause",			 rtmp_read_onpause,                NULL},

    { "FCPublish",		 rtmp_read_onfcpublish,            NULL},
	{ "FCUnpublish",	 rtmp_read_onfcunpublish,          NULL},
	{ "FCSubscribe",	 rtmp_read_onfcsubscribe,          NULL},
	{ "FCUnsubscribe",	 rtmp_read_onfcunsubscribe,        NULL},
};

int rtmp_recv_paser_invoke(rtmp_session *rtmp, bs_t *b)
{
    if (rtmp == NULL || b == NULL)
        return NET_FAIL;
    
	double transactionId = -1;
	uint8_t command[256] = {0};

	amf_object_item items[2];
	AMF_OBJECT_ITEM_VALUE(items[0], AMF_OBJECT, "command", command, sizeof(command));
	AMF_OBJECT_ITEM_VALUE(items[1], AMF_STRING, "transaction", &transactionId, sizeof(double));

	amf_read_item(b, items, sizeof(items) / sizeof(items[0]));

    LOG("command handle %s", command);

	for (int i = 0; i < sizeof(g_command_handle)/sizeof(g_command_handle[0]); i++)
    {
        if (!strncmp(g_command_handle[i].command, command, strlen(command)))
        {
			int code = g_command_handle[i].function(b, rtmp);
			if (g_command_handle[i].reply)
				g_command_handle[i].reply(rtmp, code, transactionId);
		}
    }

    return NET_SUCCESS;
}
