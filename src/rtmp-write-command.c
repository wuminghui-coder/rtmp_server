#include "amf0.h"
#include "net-common.h"
#include "rtmp-write-command.h"

int rtmp_write_command_connect(bs_t *b, double transactionId, const rtmp_connect *connect)
{
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

    amf_write_string(b, "connect", strlen("connect"));
    amf_write_double(b, transactionId);

    amf_write_object(b);
	amf_write_NamedString(b, "app", strlen("app"), connect->app, strlen(connect->app));
	amf_write_NamedString(b, "flashVer", strlen("flashVer"), connect->flashver, strlen(connect->flashver));

	if (connect->tcUrl[0]) 
        amf_write_NamedString(b, "tcUrl", strlen("tcUrl"), connect->tcUrl, strlen(connect->tcUrl));

	if (connect->swfUrl[0]) 
        amf_write_NamedString(b, "swfUrl",  strlen("swfUrl"), connect->swfUrl, strlen(connect->swfUrl));
		
	if (connect->pageUrl[0]) 
        amf_write_NamedString(b, "pageUrl",  strlen("pageUrl"), connect->pageUrl, strlen(connect->pageUrl));

	amf_write_NamedBoolean(b, "fpad",           strlen("fpad"),         connect->fpad);
	amf_write_NamedDouble(b,  "capabilities",   strlen("capabilities"), connect->capabilities);
	amf_write_NamedDouble(b,  "audioCodecs",    strlen("audioCodecs"), connect->audioCodecs);
	amf_write_NamedDouble(b,  "videoCodecs",    strlen("videoCodecs"), connect->videoCodecs);
	amf_write_NamedDouble(b,  "videoFunction",  strlen("videoFunction"), connect->videoFunction);
	amf_write_NamedDouble(b,  "objectEncoding", strlen("objectEncoding"), connect->encoding);
	amf_write_objectEnd(b);

	return bs_pos(b) - start_pos;
}

int rtmp_write_command_connect_reply(bs_t *b, double transactionId, const char* fmsver, double capabilities, const char* code, const char* level, const char* description, double encoding)
{
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

	amf_write_string(b, "_result", strlen("_result"));
	amf_write_double(b, transactionId);

	amf_write_object(b);
	amf_write_NamedString(b, "fmsVer", strlen("fmsVer"), fmsver, strlen(fmsver));
	amf_write_NamedDouble(b,  "capabilities", strlen("capabilities"), capabilities);
	amf_write_NamedDouble(b,  "mode", strlen("mode"), 1);
	amf_write_objectEnd(b);

	amf_write_object(b);
	amf_write_NamedString(b, "level", strlen("level"), level, strlen(level));
	amf_write_NamedString(b, "code", strlen("code"), code, strlen(code));
	amf_write_NamedString(b, "description", strlen("description"), description, strlen(description));
	amf_write_NamedDouble(b, "objectEncoding", strlen("objectEncoding"), encoding);
	amf_write_objectEnd(b);

	return bs_pos(b) - start_pos;
}

int rtmp_write_command_stream(bs_t *b, double transactionId)
{
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);
	amf_write_string(b, "createStream", strlen("createStream"));
	amf_write_double(b, transactionId);
	amf_write_null(b);

	return bs_pos(b) - start_pos;
}

int rtmp_write_command_stream_reply(bs_t *b, double transactionId, double stream_id)
{
    if (b == NULL)
        return NET_FAIL;    
    int start_pos = bs_pos(b);
	amf_write_string(b, "_result", strlen("_result"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_double(b, stream_id);

	return bs_pos(b) - start_pos;
}

int rtmp_write_command_stream_length(bs_t *b, double transactionId, const char* stream_name)
{
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);
	
    amf_write_string(b, "getStreamLength", strlen("getStreamLength"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_string(b, stream_name, strlen(stream_name));

	return bs_pos(b) - start_pos;
}

int rtmp_write_command_stream_length_reply(bs_t *b, double transactionId, double duration)
{   
    if (b == NULL)
        return NET_FAIL;    
    int start_pos = bs_pos(b);
	
    amf_write_string(b, "_result", strlen("_result"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_double(b, duration);
	
    return bs_pos(b) - start_pos;
}

int rtmp_write_command_error(bs_t *b, double transactionId, const char* code, const char* level, const char* description)
{
    if (b == NULL)
        return NET_FAIL;

    int start_pos = bs_pos(b);

	amf_write_string(b, "_error", strlen("_error"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	
	amf_write_object(b);
	amf_write_NamedString(b, "code", strlen("code"), code, strlen(code));
	amf_write_NamedString(b, "level", strlen("level"), level, strlen(level));
	amf_write_NamedString(b, "description", strlen("description"), description, strlen(description));
	amf_write_objectEnd(b);

	return bs_pos(b) - start_pos;
}

int rtmp_write_play(bs_t *b, double transactionId, const char* name, double start, double duration, int reset)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "play", strlen("play"));
	amf_write_double(b, transactionId);
	amf_write_null(b);

	amf_write_string(b, name, strlen(name));
	amf_write_double(b, start);
	amf_write_double(b, duration);
	amf_write_boolean(b, reset);

	return bs_pos(b) - start_pos;
}

int rtmp_write_delete_stream(bs_t *b,  double transactionId, double streamId)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "deleteStream", strlen("deleteStream"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_double(b, streamId);

	return bs_pos(b) - start_pos;
}

int rtmp_write_close_stream(bs_t *b,  double transactionId, double streamId)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "closeStream", strlen("closeStream"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_double(b, streamId);

	return bs_pos(b) - start_pos;
}

int rtmp_write_receive_audio(bs_t *b,  double transactionId, int enable)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "receiveAudio", strlen("receiveAudio"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_boolean(b, enable);

	return bs_pos(b) - start_pos;
}

int rtmp_write_receive_video(bs_t *b,  double transactionId, int enable)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "receiveVideo", strlen("receiveVideo"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_boolean(b, enable);

	return bs_pos(b) - start_pos;
}

int rtmp_write_publish(bs_t *b,  double transactionId, const char* stream_name, const char* stream_type)
{
	if (b == NULL || NULL == stream_name || NULL == stream_type)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	const char* command = "publish";

	amf_write_string(b, "publish", strlen("publish"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_string(b, stream_name, strlen(stream_name)); 
	amf_write_string(b, stream_type, strlen(stream_type));

	return bs_pos(b) - start_pos;
}

int rtmp_write_seek(bs_t *b,  double transactionId, double ms)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "seek", strlen("seek"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_double(b, ms);

	return bs_pos(b) - start_pos;
}

int rtmp_write_pause(bs_t *b,  double transactionId, int pause, double ms)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "pause", strlen("pause"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_boolean(b, pause);
	amf_write_double(b, ms);

	return bs_pos(b) - start_pos;
}

int rtmp_write_release_stream(bs_t *b,  double transactionId, const char* stream_name)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "releaseStream", strlen("releaseStream"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_string(b, stream_name, strlen(stream_name));

	return bs_pos(b) - start_pos;
}

int rtmp_write_fcpublish(bs_t *b,  double transactionId, const char* stream_name)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "FCPublish", strlen("FCPublish"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_string(b, stream_name, strlen(stream_name));

	return bs_pos(b) - start_pos;
}

int rtmp_write_fcunpublish(bs_t *b,  double transactionId, const char* stream_name)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "FCUnpublish", strlen("FCUnpublish"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_string(b, stream_name, strlen(stream_name));

	return bs_pos(b) - start_pos;
}

int rtmp_write_fcsubscribe(bs_t *b,  double transactionId, const char* subscribepath)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "FCSubscribe", strlen("FCSubscribe"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_string(b, subscribepath, strlen(subscribepath)); 

	return bs_pos(b) - start_pos;
}

int rtmp_write_fcunsubscribe(bs_t *b,  double transactionId, const char* subscribepath)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "FCUnsubscribe", strlen("FCUnsubscribe"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_string(b, subscribepath, strlen(subscribepath)); 

	return bs_pos(b) - start_pos;
}

int rtmp_write_onbwdone(bs_t *b,  double transactionId, double bandwidth)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "onBWDone", strlen("onBWDone"));
	amf_write_double(b, transactionId);
	amf_write_null(b);
	amf_write_double(b, bandwidth);

	return bs_pos(b) - start_pos;
}

int rtmp_write_checkbw(bs_t *b,  double transactionId)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "_checkbw", strlen("_checkbw"));
	amf_write_double(b, transactionId);
	amf_write_null(b);

    return bs_pos(b) - start_pos;
}

int rtmp_write_onstatus(bs_t *b,  double transactionId, const char* level, const char* code, const char* description)
{
	if (b == NULL || NULL == level || NULL == code || NULL == description)
		return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "onStatus", strlen("onStatus"));
	amf_write_double(b, transactionId);
	amf_write_null(b);

	amf_write_object(b);
	amf_write_NamedString(b, "level", strlen("level"), level, strlen(level));
	amf_write_NamedString(b, "code", strlen("code"),  code, strlen(code));
	amf_write_NamedString(b, "description", strlen("description"), description, strlen(description));
	amf_write_objectEnd(b);

	return bs_pos(b) - start_pos;
}

int rtmp_write_rtmpsampleaccess(bs_t *b)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "|RtmpSampleAccess", strlen("|RtmpSampleAccess"));
	amf_write_boolean(b, 1);
	amf_write_boolean(b, 1);

	return bs_pos(b) - start_pos;
}

int rtmp_write_onMetadata(bs_t *b)
{
	if (b == NULL)
        return NET_FAIL;

	int start_pos = bs_pos(b);

	amf_write_string(b, "onMetaData", strlen("onMetaData"));

	amf_write_object(b);
	amf_write_NamedString(b, "Server", strlen("Server"), "nginx-rtmp-module", strlen("nginx-rtmp-module"));
	amf_write_NamedDouble(b,  "width",   strlen("width"),  1280);
	amf_write_NamedDouble(b,  "height",  strlen("height"),  720);
	amf_write_NamedDouble(b,  "displayWidth",   strlen("displayWidth"),  1280);
	amf_write_NamedDouble(b,  "displayHeight",   strlen("displayHeight"),  720);
	amf_write_NamedDouble(b,  "duration",   strlen("duration"),  0);
	amf_write_NamedDouble(b,  "framerate",   strlen("framerate"),  25);
	amf_write_NamedDouble(b,  "fps",   strlen("fps"),  25);
	amf_write_NamedDouble(b,  "videodatarate",   strlen("videodatarate"),  0);
	amf_write_NamedDouble(b,  "videocodecid",   strlen("videocodecid"),  7);
	amf_write_NamedDouble(b,  "audiodatarate",   strlen("audiodatarate"),  0);
	amf_write_NamedDouble(b,  "audiocodecid",   strlen("audiocodecid"),  0);
	amf_write_NamedString(b, "profile", strlen("profile"), "", 0);
	amf_write_NamedString(b, "level", strlen("level"),  "", 0);
	amf_write_objectEnd(b);
	return bs_pos(b) - start_pos;
}
