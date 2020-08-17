#ifndef WEBRTCDEF_H
#define WEBRTCDEF_H
#include <string>

//////////////////////////////////////////////////////////////////////////




//////////////////////////////////////////////////////////////////////////
extern const char kAudioLabel[];
extern const char kVideoLabel[];
extern const char kStreamLabel[];

const static char* JanusComRecieveState[] = { "success","error", "ack", "event", "webrtcup", "hangup"};
//错误信息
#define JanusReturnError JanusComRecieveState[1]
//成功信息
#define JanusReturnSuccess JanusComRecieveState[0]
//普通应答信息
#define JanusReturnAck JanusComRecieveState[2]
//事件信息，数据格式需要具体分析
#define JanusReturnEvent JanusComRecieveState[3]
//webrtc启动成功信息
#define JanusReturnWebrtcUP JanusComRecieveState[4]
//webrtc挂断信息
#define JanusReturnHangUP JanusComRecieveState[5]


enum JanusComState {
	jcsWSConnect=0,
	jcsSession, 
	jcsHandleid,
	jcsWatch,
	jcsAnswer,
	jcsTrickle,
	jscStop,
	jcsAlive,
};

enum CallbackID {
    MEDIA_CHANNELS_INITIALIZED = 1,
    PEER_CONNECTION_CLOSED,
    SEND_MESSAGE_TO_PEER,
    NEW_STREAM_ADDED,
    STREAM_REMOVED,
    SIGNALING_POLLING,
    UPDATE_FRAME,

    EVT_WATCH,
    EVT_POLLING,
};

struct WebRTCReport
{
	int rtt;
	float packetLossRate;
	int bitrate;
	int frameRate;
};


void debugstr(const char* str);

void logwebfunction(const std::string& function,
	const std::string& file,
	int line);

void logwebstr(const std::string& function,
	const std::string& file,
	int line, char* str);

#define WEBRTC_LOGGER_FUNC logwebfunction(__FUNCTION__, __FILE__, __LINE__);
#define WEBRTC_LOGGER_STR(label) logwebstr(__FUNCTION__, __FILE__, __LINE__, label);
#endif // WEBRTCDEF_H
