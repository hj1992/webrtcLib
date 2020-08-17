//
//  SignalingSession.hpp
//  CppRest
//
//  Created by Wang Xin on 15/03/2017.
//  Copyright © 2017 BoostSwift. All rights reserved.
//

#ifndef SignalingSession_H
#define SignalingSession_H


#include <string>
#ifdef ANDROID
#include "cJSON.h"
#endif
#include "CWebSocket.h"
#include "httpmodelsrest.h"
#include "baseinstanceabstract.h"
#include <boost/interprocess/sync/interprocess_semaphore.hpp>


//janus 协议地址https://janus.conf.meetecho.com/docs/rest.html#WS

//class VMInstanceData /*: public JsonDataBase*/ {
//public:
//	int id;         // 序号
//	int cpu;        // CPU核数量
//	int memory;     // 内存大小
//	int diskSize;   // 硬盘大小
//	int vmsnId;     // 虚拟机连接Id
//	int serverPort; // 连接服务器端口
//	//int turnPort;   // 转发服务器端口
//	//int stunPort;   // 穿透服务器端口
//	int statusValue; // 状态value
//	uint64_t createTime;
//	std::string status;
//	std::string vmName;
//	std::string vmUuid;
//	std::string innerIP1;
//	std::string description;
//	std::string hostIp;
//	
//	//std::string turnAddress;      // 转发服务器地址
//	//std::string stunAddress;      // 穿透服务器地址
//	std::string instanceKey;      // 实例key
//	int serverDomainPort;
//	std::string serverDomainUrl;
//	//webrtc 连接信息
//	std::string janusIP;    // janus http address
//	int janusPort;  // janus http port
//	std::string iceServerInfo;
//	//webrtc end
//};

class WebrtcClient;

class SignalingSession{

public:
	SignalingSession(WebrtcClient* webrtcClient, const gwecom::app::VappInstanceData & vmInstance, gwecom::gweNetwork::BaseInstanceAbstract* instance);
	~SignalingSession();
	bool createSession();
	bool createHandler();
	bool getJanusInfo();
	bool keepAlive();
	bool sendMessage(cJSON* msgBody);
	bool sendTrickle(cJSON* candidate);
	bool sendAnswer(std::string sdp);
	bool startWatch(std::string instancekey);
	bool stopWatch();
	void waitWebrtcHungUP();
    void ResetBaseInstance(gwecom::gweNetwork::BaseInstanceAbstract* baseInstance);
public:
	const gwecom::app::VappInstanceData & getVMInstance() const;
	void setVMInstance(const gwecom::app::VappInstanceData & value);

	const std::string& getSessionId() const;
	void setSessionId( std::int64_t  value);

	const std::string& getHandlerId() const;
	void setHandlerId( std::int64_t  value);

	void onRecievejanusData(WSDataType wsdt, const char* data, int datalen);

protected:
	std::string genTransaction() const;

private:
	gwecom::gweNetwork::BaseInstanceAbstract* m_instance;
	gwecom::app::VappInstanceData  vmInstance;
	JanusComState m_jcs;
	std::string sessionId;
	 std::int64_t  isessionId;

	std::string handlerId;
	 std::int64_t  ihandlerId;
	//CURL* curl;
	std::string baseUrl;
	CWebSocket* janusws;
	bool m_useWebSocket = true;//使用websocket与janus通讯
	std::string m_sdpinfo;

	JanusComState getNextStep();
	WebrtcClient* m_webrtcClient;
	boost::interprocess::interprocess_semaphore* m_webrtcStopSem;
	bool m_bwaitwebrtcStop;

	bool NextStep();
};

#endif /* SignalingSession_hpp */
