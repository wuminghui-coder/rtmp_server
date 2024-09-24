#ifndef __RTMP_CONNECT_MESSAGE_H__
#define __RTMP_CONNECT_MESSAGE_H__
#include "net-common.h"

enum rtmp_audio_codec_t
{
	SUPPORT_SND_NONE		= 0x0001, // Raw sound, no compression
	SUPPORT_SND_ADPCM		= 0x0002, // ADPCM compression
	SUPPORT_SND_MP3			= 0x0004, // mp3 compression
	SUPPORT_SND_INTEL		= 0x0008, // Not used
	SUPPORT_SND_UNUSED		= 0x0010, // Not used
	SUPPORT_SND_NELLY8		= 0x0020, // NellyMoser at 8-kHz compression
	SUPPORT_SND_NELLY		= 0x0040, // NellyMoser compression (5, 11, 22, and 44 kHz)
	SUPPORT_SND_G711A		= 0x0080, // G711A sound compression (Flash Media Server only)
	SUPPORT_SND_G711U		= 0x0100, // G711U sound compression (Flash Media Server only)
	SUPPORT_SND_NELLY16		= 0x0200, // NellyMouser at 16-kHz compression
	SUPPORT_SND_AAC			= 0x0400, // Advanced audio coding
	SUPPORT_SND_SPEEX		= 0x0800, // Speex Audio
	SUPPORT_SND_ALL			= 0x0FFF, // All RTMP-supported audio codecs
};

enum rtmp_video_codec_t
{
	SUPPORT_VID_UNUSED		= 0x0001, // Obsolete value
	SUPPORT_VID_JPEG		= 0x0002, // Obsolete value
	SUPPORT_VID_SORENSON	= 0x0004, // Sorenson Flash video
	SUPPORT_VID_HOMEBREW	= 0x0008, // V1 screen sharing
	SUPPORT_VID_VP6			= 0x0010, // On2 video (Flash 8+)
	SUPPORT_VID_VP6ALPHA	= 0x0020, // On2 video with alpha channel
	SUPPORT_VID_HOMEBREWV	= 0x0040, // Screen sharing version 2 (Flash 8+)
	SUPPORT_VID_H264		= 0x0080, // H264 video
	SUPPORT_VID_ALL			= 0x00FF, // All RTMP-supported video codecs
};

enum rtmp_video_function_t
{
	SUPPORT_VID_CLIENT_SEEK	= 1, // Indicates that the client can perform frame-accurate seeks.
};

typedef struct rtmp_connect_t
{
	char app[128]; // Server application name, e.g.: testapp
	char flashver[64]; // Flash Player version, FMSc/1.0
	char type[128];
	char swfUrl[256]; // URL of the source SWF file
	char tcUrl[256]; // URL of the Server, rtmp://host:1935/testapp/instance1
	uint8_t fpad; // boolean: True if proxy is being used.
	double capabilities; // double default: 15
	double audioCodecs; // double default: 4071
	double videoCodecs; // double default: 252
	double videoFunction; // double default: 1
	double encoding;
	char pageUrl[256]; // http://host/sample.html
} rtmp_connect;

int rtmp_write_command_connect(bs_t *b, double transactionId, const rtmp_connect *connect);
int rtmp_write_command_connect_reply(bs_t *b, double transactionId, const char* fmsver, double capabilities, const char* code, const char* level, const char* description, double encoding);
int rtmp_write_command_stream(bs_t *b, double transactionId);
int rtmp_write_command_stream_reply(bs_t *b, double transactionId, double stream_id);
int rtmp_write_command_stream_length(bs_t *b, double transactionId, const char* stream_name);
int rtmp_write_command_stream_length_reply(bs_t *b, double transactionId, double duration);
int rtmp_write_command_error(bs_t *b, double transactionId, const char* code, const char* level, const char* description);


int rtmp_write_rtmpsampleaccess(bs_t *b);
int rtmp_write_onstatus(bs_t *b,  double transactionId, const char* level, const char* code, const char* description);
int rtmp_write_checkbw(bs_t *b,  double transactionId);
int rtmp_write_onbwdone(bs_t *b,  double transactionId, double bandwidth);
int rtmp_write_fcunsubscribe(bs_t *b,  double transactionId, const char* subscribepath);
int rtmp_write_fcsubscribe(bs_t *b,  double transactionId, const char* subscribepath);
int rtmp_write_fcunpublish(bs_t *b,  double transactionId, const char* stream_name);
int rtmp_write_fcpublish(bs_t *b,  double transactionId, const char* stream_name);
int rtmp_write_release_stream(bs_t *b,  double transactionId, const char* stream_name);
int rtmp_write_pause(bs_t *b,  double transactionId, int pause, double ms);
int rtmp_write_seek(bs_t *b,  double transactionId, double ms);
int rtmp_write_publish(bs_t *b,  double transactionId, const char* stream_name, const char* stream_type);
int rtmp_write_receive_video(bs_t *b,  double transactionId, int enable);
int rtmp_write_receive_audio(bs_t *b,  double transactionId, int enable);
int rtmp_write_close_stream(bs_t *b,  double transactionId, double streamId);
int rtmp_write_delete_stream(bs_t *b,  double transactionId, double streamId);
int rtmp_write_play(bs_t *b, double transactionId, const char* name, double start, double duration, int reset);
int rtmp_write_onMetadata(bs_t *b);

#endif // !__RTMP_CONNECT_MESSAGE_H__
