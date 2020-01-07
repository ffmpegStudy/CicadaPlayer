#ifndef MEDIA_PLAYER_ERROR_DEF_H
#define MEDIA_PLAYER_ERROR_DEF_H

namespace Cicada{

	enum MediaPlayerErrType {
		////////////// 播放器 0x20000000-0x2FFFFFFF/////////////////
		MEDIA_PLAYER_ERROR_START = 0x20000000,
		// 1、播放器通用错误;
		MEDIA_PLAYER_ERROR_SYSTEM_START = 0x20010000,

		//WARNING: This code part is used by other components of the player,
		// should never add error code before MEDIA_PLAYER_ERROR_NATIVE_START.

		// 2、apsaraplayer编排层错误;
		MEDIA_PLAYER_ERROR_NATIVE_START = 0x20020000,
		MEDIA_PLAYER_ERROR_READ_PACKET,
		MEDIA_PLAYER_ERROR_BUFFER_STUFFED,

		// 3、demuxer、datasource模块错误
		MEDIA_PLAYER_ERROR_DEMUXER_START = 0x20030000,
		MEDIA_PLAYER_ERROR_DEMUXER_OPENURL,
		MEDIA_PLAYER_ERROR_DEMUXER_NO_VALID_STREAM,
		MEDIA_PLAYER_ERROR_DEMUXER_OPENSTREAM,
		MEDIA_PLAYER_ERROR_LOADING_TIMEOUT,
		MEDIA_PLAYER_ERROR_DATASOURCE_EMPTYURL,

		MEDIA_PLAYER_ERROR_DECODE_BASE = 0x20040000,
		MEDIA_PLAYER_ERROR_DECODE_VIDEO,
		MEDIA_PLAYER_ERROR_DECODE_AUDIO,

		// 4. network错误
		MEDIA_PLAYER_ERROR_NETWORK_UNKNOWN = 0x20050000,
		MEDIA_PLAYER_ERROR_NETWORK_UNSUPPORTED,
		MEDIA_PLAYER_ERROR_NETWORK_RESOLVE,
		MEDIA_PLAYER_ERROR_NETWORK_CONNECT_TIMEOUT,
		MEDIA_PLAYER_ERROR_NETWORK_COULD_NOT_CONNECT,
        MEDIA_PLAYER_ERROR_NETWORK_HTTP_400,
		MEDIA_PLAYER_ERROR_NETWORK_HTTP_403,
		MEDIA_PLAYER_ERROR_NETWORK_HTTP_404,
		MEDIA_PLAYER_ERROR_NETWORK_HTTP_4XX,
		MEDIA_PLAYER_ERROR_NETWORK_HTTP_5XX,
		MEDIA_PLAYER_ERROR_NETWORK_HTTP_RANGE,

		//5. codec 错误
		MEDIA_PLAYER_ERROR_CODEC_UNKNOWN = 0x20060000,
		MEDIA_PLAYER_ERROR_CODEC_VIDEO_NOT_SUPPORT,
		MEDIA_PLAYER_ERROR_CODEC_AUDIO_NOT_SUPPORT,
		MEDIA_PLAYER_ERROR_CODEC_VIDEO_OPEN_ERROR,
		//6. internal 错误
		MEDIA_PLAYER_ERROR_INERNAL_UNKNOWN = 0x20070000,
		MEDIA_PLAYER_ERROR_INERNAL_EXIT,

		//7.general 错误
		MEDIA_PLAYER_ERROR_GENERAL_UNKNOWN = 0x20080000,
		MEDIA_PLAYER_ERROR_GENERAL_EPERM = 0x20080001,
		MEDIA_PLAYER_ERROR_GENERAL_ENOENT = 0x20080002,
		MEDIA_PLAYER_ERROR_GENERAL_EIO = 0x20080005,

		//
		MEDIA_PLAYER_ERROR_UNKNOWN = 0x30000000 - 1,
	};

	enum MediaPlayerEventType {
        MEDIA_PLAYER_EVENT_START = 0,
        MEDIA_PLAYER_EVENT_SW_VIDEO_DECODER = MEDIA_PLAYER_EVENT_START,
        MEDIA_PLAYER_EVENT_AUDIO_CODEC_NOT_SUPPORT,
        MEDIA_PLAYER_EVENT_AUDIO_DECODER_DEVICE_ERROR,
        MEDIA_PLAYER_EVENT_VIDEO_CODEC_NOT_SUPPORT,
        MEDIA_PLAYER_EVENT_VIDEO_DECODER_DEVICE_ERROR,
        MEDIA_PLAYER_EVENT_VIDEO_RENDER_INIT_ERROR,

        MEDIA_PLAYER_EVENT_DEMUXER_TRACE_ID,
        MEDIA_PLAYER_EVENT_DEMUXER_EOF,
        MEDIA_PLAYER_EVENT_NETWORK_RETRY,
        MEDIA_PLAYER_EVENT_NETWORK_RETRY_SUCCESS,
        MEDIA_PLAYER_EVENT_CACHE_SUCCESS,
        MEDIA_PLAYER_EVENT_CACHE_ERROR,
        MEDIA_PLAYER_EVENT_SYSTEM_LOW_MEMORY,
        MEDIA_PLAYER_EVENT_DEMUXER_STARTUP_INFO,

        MEDIA_PLAYER_EVENT_SUBTITLE_SELECT_ERROR,
	};
}

#endif //MEDIA_PLAYER_ERROR_DEF_H
