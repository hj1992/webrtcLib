/**
 * @breaf ���ڴ���websocket����
 * @note  ����ʵ����client�˻�����websocket�������Զ�������
 * @author  dreamflywu
 * date   2019��5��12��
 */

#ifndef CWebSocket_H
#define CWebSocket_H

enum JanusWebSocketType {
	jwstNoSsl=0,
	jwstSsl=1,
};
#include "WebrtcDef.h"
#ifdef DEBUG_WEBRTC



#include "libwebsockets.h"

#include <signal.h>
#include <string>
#include <thread>
#include "ServerSslCaPem.h"
#ifdef _WIN32
#include "commonlib.h"
#endif
#ifdef ANDROID
#include "mobilenetworkconfig.h"
#endif
#include <vector>
#include <mutex>
#include <deque>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include "timer.h"

// * �Ự�����Ķ��󣬽ṹ������Ҫ�Զ���
#define MAX_PAYLOAD_SIZE  10 * 1024

struct session_data {
	int msg_count;
	unsigned char buf[LWS_PRE + MAX_PAYLOAD_SIZE];
	int len;
	void* user;
};

enum WSDataType {
	wsdtNotice,
	wsdtData,
};

enum WSStateType {
	wsstConnected=0,
	wsstDisconnect,
	wsstError,
};

typedef void(__stdcall* WSDataCB)(WSDataType, const char*, int, void*); //wsdtnotice int is notice type

class CWebSocket 
{
public:
	CWebSocket();
	~CWebSocket();

	bool initWebSocket(const char* HostPath, WSDataCB wsCB, void* user);
	//bool getStatue();
	bool sendData(std::string datas);

	void onConnected();
	void onError();
	void onDisConnectd();
	void onRecieve(std::string srcv);

	void startReconect();

	std::string getSendData();
	bool connect();

	bool getConnectState() {
		return m_connectState;
	}
	//bool m_waitSend;
private:
	
	bool m_connectState;
	std::thread m_thread;
	struct lws_context *context;
	struct lws *wsi;
	JanusWebSocketType m_jwst;
	std::string m_hostIP;
	int m_hostPort;
	std::string m_hostPath;
	bool m_alive;
	bool m_close;
	std::deque<std::string> m_sendList;
	std::mutex m_sendLock;

	WSDataCB m_wsCB;
	void* m_user;

	//std::string m_recved;
	
	//boost::interprocess::interprocess_semaphore* m_connectSemp;
	//boost::interprocess::interprocess_semaphore* m_sendSemp;
	ATimer<>* m_connectTimer;

	void Process();

	

	void wait();
	void post();
};

#endif // CWebSocket
#endif // !
