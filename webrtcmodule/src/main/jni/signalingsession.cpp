//
//  SignalingSession.cpp
//  CppRest
//
//  Created by Wang Xin on 15/03/2017.
//  Copyright © 2017 BoostSwift. All rights reserved.
//
#include "signalingsession.h"
#include "cJSON.h"
#include <ctime>
#include <functional>
#include "WebrtcClient.h"


using namespace std;


void __stdcall janusWSDataCB(WSDataType wsdt, const char* data, int datalen, void* user) {
	SignalingSession* _signal = (SignalingSession*)user;
    if(_signal)
	_signal->onRecievejanusData(wsdt, data, datalen);
 }

SignalingSession::SignalingSession(WebrtcClient* webrtcClient, const gwecom::app::VappInstanceData & _vmInstance, gwecom::gweNetwork::BaseInstanceAbstract* instance)
	: vmInstance(_vmInstance), m_instance(instance), m_webrtcClient(webrtcClient)
	{

	//curl_global_init(CURL_GLOBAL_ALL);
    janusws = NULL;
	m_webrtcStopSem = new boost::interprocess::interprocess_semaphore(0);
	m_bwaitwebrtcStop = false;
	baseUrl = _vmInstance.janusIP + ":" + std::to_string(_vmInstance.janusPort) + "/janus";
    LOGI("SignalingSession baseUrl, %s\n",baseUrl.c_str());
    if (m_useWebSocket)
    {
		janusws = new CWebSocket();
        if (janusws)
        {
            janusws->initWebSocket(baseUrl.c_str(), janusWSDataCB, this);
        }
		
	}
    m_jcs=jcsWSConnect;
}

SignalingSession::~SignalingSession()
{
	delete m_webrtcStopSem;

	if (m_useWebSocket) {
		delete janusws;
	}
}

const gwecom::app::VappInstanceData & SignalingSession::getVMInstance() const {
	return vmInstance;
}

void SignalingSession::setVMInstance(const gwecom::app::VappInstanceData & value) {
	vmInstance = value;
}

const std::string& SignalingSession::getSessionId() const {
	return sessionId;
}

void SignalingSession::setSessionId( std::int64_t  value) {
	isessionId = value;
	sessionId = std::to_string(value);
}

const std::string& SignalingSession::getHandlerId() const {
	return handlerId;
}

void SignalingSession::setHandlerId( std::int64_t  value) {
	handlerId = std::to_string(value);
	ihandlerId = value;
}

void SignalingSession::onRecievejanusData(WSDataType wsdt, const char * data, int datalen)
{
	//std::string janusReturn = data;
	if (wsdt == wsdtNotice)
	{
        LOGI("onRecievejanusData_0, %d\n",wsdt);
		WSStateType wsst = (WSStateType)datalen;
        LOGI("onRecievejanusData, %d\n",wsst);
		switch (wsst)
		{
		case wsstConnected:
		{
			if(m_instance)
				m_instance->onJanusWSConnected();
			NextStep();
		}
			break;
		case wsstDisconnect:
		{
			if(m_instance)
				m_instance->onJanusWSError();
			if (m_bwaitwebrtcStop)
				m_webrtcStopSem->post();
		}
			break;
		case wsstError:
		{
			if(m_instance)
				m_instance->onJanusWSError();
			if (m_bwaitwebrtcStop)
				m_webrtcStopSem->post();
		}
			break;
		default:
			break;
		}
		return;
	}



	if (datalen == 0)
		return;
    if (!data)
        return;
    
    LOGI("onRecievejanusData_3, %d__%d\n", datalen, janusws->getConnectState());
    LOGI("onRecievejanusData_3, %s\n", data);
	cJSON* jsonJanusReturn = cJSON_Parse(data);
	if (!jsonJanusReturn)
		return;
	const char* janusResult = cJSON_GetStringFromObj(jsonJanusReturn, "janus");
	if (strcmp(janusResult, JanusReturnError) == 0) {
		std::string errorstr = data;
		if(m_instance)
		  m_instance->onJanusRecieveError(errorstr);
		cJSON_Delete(jsonJanusReturn);
		return;
	}
	if (strcmp(janusResult, JanusReturnAck) == 0) {
		cJSON_Delete(jsonJanusReturn);
		return;
	}
	if (strcmp(janusResult, JanusReturnSuccess) == 0) {
		//createsession\createhandler返回该类数据
		switch (m_jcs)
		{
		case jcsSession: {
			cJSON* idobj = cJSON_GetObjectItem(jsonJanusReturn, "data");
			std::int64_t isessionid = cJSON_GetDoubleFromObj(idobj, "id");
			setSessionId(isessionid);			
		}
			break;
		case jcsHandleid: {
			cJSON* idobj = cJSON_GetObjectItem(jsonJanusReturn, "data");
			std::int64_t ihandleid = cJSON_GetDoubleFromObj(idobj, "id");
			setHandlerId(ihandleid);
		}
			break;
		default:
			break;
		}
	}
	if (strcmp(janusResult, JanusReturnEvent) == 0) {
		
			cJSON* jsep = cJSON_GetObjectItem(jsonJanusReturn, "jsep");

			if (!jsep) {
				return;
			}

			m_sdpinfo = cJSON_GetStringFromObj(jsep, "sdp");
			if (m_jcs == jcsWatch) {
				m_webrtcClient->onRecieveSDP(m_sdpinfo);
			}
	}

	if (strcmp(janusResult, JanusReturnWebrtcUP) == 0) {
	    if(m_instance)
		m_instance->onJanusWebrtcUP();
	}
	if (strcmp(janusResult, JanusReturnHangUP) == 0) { 
		
        cJSON* jsep = cJSON_GetObjectItem(jsonJanusReturn, "reason");
        if (m_instance
            && jsep)
        {
            std::string errorstr = jsep->valuestring;
            if (!errorstr.empty())
            {
                m_instance->onJanusRecieveError(errorstr);
            }
            
        }

        m_webrtcStopSem->post();
	}
	LOGI("onRecievejanusData_3, begin NextStep\n");
	NextStep();
    LOGI("onRecievejanusData_3, end NextStep\n");
	if (jsonJanusReturn)
		cJSON_Delete(jsonJanusReturn);

}

std::string SignalingSession::genTransaction() const {
	static const char* s_pool = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
	const int pool_size = strlen(s_pool);
	const int transaction_length = 12;
	char data[transaction_length];
	for (int i = 0; i < transaction_length; i++) {
		data[i] = s_pool[rand() % pool_size];
	}

	return std::string(data, transaction_length);
}

JanusComState SignalingSession::getNextStep()
{
	int nextstep = (int)m_jcs;
	return JanusComState(nextstep++);
}

bool SignalingSession::NextStep()
{
    LOGI("NextStep, %d\n",m_jcs);
	switch (m_jcs)
	{
	case jcsWSConnect:
        LOGI("createSession, %d\n",m_jcs);
		return createSession();
		break;
	case jcsSession:
		return createHandler();
		break;
	case jcsHandleid:
		keepAlive();
		return startWatch(vmInstance.instanceKey);
		break;
	case jcsWatch:
		break;
	case jcsAnswer:
		break;
	case jcsTrickle:
		break;
	case jscStop:
		break;
	case jcsAlive:
		break;
	default:
		break;
	}
	return true;
}

size_t WriteCallback(void *buffer, size_t size, size_t nmemb, std::string *userp) {
	userp->append((char*)buffer, size * nmemb);
	return size * nmemb;
}



bool SignalingSession::createSession() {
	m_sdpinfo = "";
	m_jcs = jcsSession;
	if (m_useWebSocket) {
		bool ret = false;
		cJSON* pJsonObj = cJSON_CreateObject();
		cJSON_AddStringToObject(pJsonObj, "janus", "create");
		cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());


		std::string json = cJSON_Print(pJsonObj);
		ret = janusws->sendData(json.c_str());
		cJSON_Delete(pJsonObj);
		return ret;
	}
	else {
		bool ret = false;
		/*CLibcurl libcurl;


		cJSON* pJsonObj = cJSON_CreateObject();
		cJSON_AddStringToObject(pJsonObj, "janus", "create");
		cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());


		std::string json = cJSON_Print(pJsonObj);

		libcurl.AddHeader("Content-Type", "application/json;charset=UTF-8");

		ret = libcurl.Post(baseUrl.c_str(), json.c_str(), json.size());
		cJSON_Delete(pJsonObj);
		if (!ret) {

			return ret;
		}
		const char* str = libcurl.GetResponsPtr();
		pJsonObj = cJSON_Parse(str);
		std::string retJanus = cJSON_GetStringFromObj(pJsonObj, "janus");

		if (strcmp(retJanus.c_str(), "success") != 0) {
			cJSON_Delete(pJsonObj);
			return ret;
		}


		cJSON* idobj = cJSON_GetObjectItem(pJsonObj, "data");
		INT64 isessionid = cJSON_GetDoubleFromObj(idobj, "id");
		setSessionId(isessionid);
		cJSON_Delete(pJsonObj);
		ret = true;*/
		return ret; 
	}
	
	
}

bool SignalingSession::createHandler() {
	m_jcs = jcsHandleid;
	if (m_useWebSocket) {
		bool ret = false;
		

		cJSON* pJsonObj = cJSON_CreateObject();
		cJSON_AddStringToObject(pJsonObj, "janus", "attach");
		cJSON_AddStringToObject(pJsonObj, "plugin", "janus.plugin.streaming");
		//cJSON_AddStringToObject(pJsonObj, "plugin", "janus.plugin.virtual.app");
		cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str()); 
		cJSON_AddNumberToObject(pJsonObj, "session_id", isessionId);
		//std::string json = jmessage.toStyledString();
		//json = json.substr(0, json.size() - 1);


		std::string json = cJSON_Print(pJsonObj);

		
		ret = janusws->sendData(json);
		cJSON_Delete(pJsonObj); 
		return ret;
		
	}
	else {
		bool ret = false;

		return ret;
	}
}

bool SignalingSession::getJanusInfo()
{
	if (m_useWebSocket) {
		bool ret = false;
		cJSON* pJsonObj = cJSON_CreateObject();
		cJSON_AddStringToObject(pJsonObj, "janus", "info");
		cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());


		std::string json = cJSON_Print(pJsonObj);
		ret = janusws->sendData(json.c_str());
		cJSON_Delete(pJsonObj);
		return ret;

	}
	return false;
}

bool SignalingSession::keepAlive()
{
	//WEBRTC_LOGGER_FUNC
	if (m_useWebSocket) {

		bool ret = false;

		auto now = std::time(nullptr) * 1000;
		static auto _ti = now;

		std::string url = baseUrl + "/janus";
		url += "/";
		url += sessionId;

		url += "?";
		url += "maxev=1";
		url += "&rid=";
		url += std::to_string(now);
		url += "&_=";
		url += std::to_string(++_ti); 

		cJSON* pJsonObj = cJSON_CreateObject();
		cJSON_AddStringToObject(pJsonObj, "janus", "keepalive");
		cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());
		cJSON_AddNumberToObject(pJsonObj, "session_id", isessionId);
		/*cJSON_AddNumberToObject(pJsonObj, "handle_id", ihandlerId);
		cJSON_AddNumberToObject(pJsonObj, "maxev", 1);
		cJSON_AddStringToObject(pJsonObj, "rid", std::to_string(now).c_str());*/

		std::string json = cJSON_Print(pJsonObj);
		cJSON_Delete(pJsonObj);
		//ret = libcurl.Post((char*)url.c_str(), json.c_str(), json.size());
		ret = janusws->sendData(json);
		return ret;

	}
	else {
		bool ret = false;
		return ret;
	}
	
}

template <class T>
void logstr1(T label) {
	time_t now = time(nullptr);
	struct tm* current_time = localtime(&now);
	char buf[1024];
	strftime(buf, sizeof(buf), " %Y-%m-%d %H:%M:%S",
		current_time);
	std::string logtxt = std::string("[webrtcclient ") + buf + "] " +
		"file:" + __FILE__ + "function:" + __FUNCTION__ +
		" lines:" + std::to_string(__LINE__) + label;
}

bool SignalingSession::sendMessage(cJSON* msgBody) {
	if (m_useWebSocket) {
		bool ret = false;

		cJSON* pJsonObj = cJSON_CreateObject();
		cJSON_AddStringToObject(pJsonObj, "janus", "message");
		cJSON_AddItemToObject(pJsonObj, "body", msgBody);
		cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());
		cJSON_AddNumberToObject(pJsonObj, "session_id", isessionId);
		cJSON_AddNumberToObject(pJsonObj, "handle_id", ihandlerId);

		std::string json = cJSON_Print(pJsonObj);
		cJSON_Delete(pJsonObj);
		//ret = libcurl.Post((char*)url.c_str(), json.c_str(), json.size());
		ret = janusws->sendData(json); 

		return ret;
		/*std::string strret = std::string(sendAck.c_str());
		
		pJsonObj = cJSON_Parse(strret.c_str());
		std::string retJanus = cJSON_GetStringFromObj(pJsonObj, "janus");

		if (strcmp(retJanus.c_str(), "ack") != 0) {
			if (strcmp(retJanus.c_str(), "event") == 0) {
				cJSON* jsep = cJSON_GetObjectItem(pJsonObj, "jsep");

				if (!jsep) {
					return ret;
				}

				m_sdpinfo = cJSON_GetStringFromObj(jsep, "sdp");
			}
		}
		
		

		cJSON_Delete(pJsonObj);*/
		ret = true;
		return ret;
	}
	else {
		bool ret = false;
		//std::string url = baseUrl + "/janus";
		//url += "/";
		//url += sessionId;
		//url += "/";
		//url += handlerId;


		//CLibcurl libcurl;
		//libcurl.AddHeader("Content-Type", "application/json;charset=UTF-8");

		//cJSON* pJsonObj = cJSON_CreateObject();
		//cJSON_AddStringToObject(pJsonObj, "janus", "message");

		//cJSON_AddItemToObject(pJsonObj, "body", msgBody);
		//cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());

		////std::string json = jmessage.toStyledString();
		////json = json.substr(0, json.size() - 1);

		//std::string json = cJSON_Print(pJsonObj);
		//cJSON_Delete(pJsonObj);
		//ret = libcurl.Post((char*)url.c_str(), json.c_str(), json.size());
		//if (!ret)
		//	return ret;

		//std::string strret = std::string(libcurl.GetResponsPtr()); 
		//pJsonObj = cJSON_Parse(strret.c_str());
		//std::string retJanus = cJSON_GetStringFromObj(pJsonObj, "janus");

		//if (strcmp(retJanus.c_str(), "ack") != 0) {
		//	cJSON_Delete(pJsonObj);
		//	return ret;
		//}

		//cJSON_Delete(pJsonObj);
		//ret = true;
		return ret;
	}
	
}

bool SignalingSession::sendTrickle(cJSON* candidate) {
	m_jcs = jcsTrickle;
	WEBRTC_LOGGER_FUNC;
	if (m_useWebSocket) {
		bool ret = false;

		cJSON* pJsonObj = cJSON_CreateObject();
		cJSON_AddStringToObject(pJsonObj, "janus", "trickle");
		cJSON_AddItemToObject(pJsonObj, "candidate", candidate);
		cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());
		cJSON_AddNumberToObject(pJsonObj, "session_id", isessionId);
		cJSON_AddNumberToObject(pJsonObj, "handle_id", ihandlerId);

		//std::string json = jmessage.toStyledString();
		//json = json.substr(0, json.size() - 1);

		std::string json = cJSON_Print(pJsonObj);
		cJSON_Delete(pJsonObj);
		
		ret = janusws->sendData(json); 
		return ret;
		/*std::string strret = std::string(trickleData.c_str());
		pJsonObj = cJSON_Parse(strret.c_str());
		if (!pJsonObj)
			return false;
		std::string retJanus = cJSON_GetStringFromObj(pJsonObj, "janus");
		cJSON_Delete(pJsonObj);
			if (strcmp(retJanus.c_str(), "success") != 0) {

				return ret;
			}
		ret = true;
		return ret;*/
	}
	else {
		bool ret = false;
		//std::string url = baseUrl + "/janus";
		//url += "/";
		//url += sessionId;
		//url += "/";
		//url += handlerId;

		//CLibcurl libcurl;
		//libcurl.AddHeader("Content-Type", "application/json;charset=UTF-8");

		//cJSON* pJsonObj = cJSON_CreateObject();
		//cJSON_AddStringToObject(pJsonObj, "janus", "trickle");
		//cJSON_AddItemToObject(pJsonObj, "candidate", candidate);
		//cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());

		////std::string json = jmessage.toStyledString();
		////json = json.substr(0, json.size() - 1);

		//std::string json = cJSON_Print(pJsonObj);
		//cJSON_Delete(pJsonObj);
		//ret = libcurl.Post((char*)url.c_str(), json.c_str(), json.size());
		//if (!ret)
		//	return ret;
		//std::string strret = std::string(libcurl.GetResponsPtr());
		//pJsonObj = cJSON_Parse(strret.c_str());
		//std::string retJanus = cJSON_GetStringFromObj(pJsonObj, "janus");
		//cJSON_Delete(pJsonObj); 
		//	if (strcmp(retJanus.c_str(), "success") != 0) {

		//		return ret;
		//	}
		//ret = true;
		return ret;
	}
}

bool SignalingSession::sendAnswer(std::string sdp) {
	m_jcs = jcsAnswer;
	if (m_useWebSocket) {
		bool ret = false;
		cJSON* jespJson = cJSON_CreateObject();
		cJSON_AddStringToObject(jespJson, "type", "answer");
		cJSON_AddStringToObject(jespJson, "sdp", sdp.c_str());
		
		//std::string jesp = cJSON_Print(jespJson);


		//Json::Value msgBody;
		//msgBody["request"] = "start";

		cJSON* msgJson = cJSON_CreateObject();
		cJSON_AddStringToObject(msgJson, "request", "start");

		cJSON* pJsonObj = cJSON_CreateObject();
		cJSON_AddStringToObject(pJsonObj, "janus", "message");
		cJSON_AddItemToObject(pJsonObj, "jsep", jespJson);
		cJSON_AddItemToObject(pJsonObj, "body", msgJson);
		cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());
		cJSON_AddNumberToObject(pJsonObj, "session_id", isessionId);
		cJSON_AddNumberToObject(pJsonObj, "handle_id", ihandlerId);
		//std::string json = jmessage.toStyledString();
		//json = json.substr(0, json.size() - 1);

		std::string json = cJSON_Print(pJsonObj);
		cJSON_Delete(pJsonObj);
		ret = janusws->sendData(json); 
		return ret;
		/*const char* strret = answerData.c_str();

		pJsonObj = cJSON_Parse(strret);
		if (!pJsonObj) {
			return ret;
		}
		std::string retJanus = cJSON_GetStringFromObj(pJsonObj, "janus");
		cJSON_Delete(pJsonObj);
		if (strcmp(retJanus.c_str(), "ack") != 0) {

			return ret;
		}

		ret = true;
		return ret;*/
	}
	else {
		bool ret = false;
		//std::string url = baseUrl + "/janus";
		//url += "/";
		//url += sessionId;
		//url += "/";
		//url += handlerId;


		////Json::Value jesp;
		////jesp["type"] = "answer";
		////jesp["sdp"] = sdp;

		//CLibcurl libcurl;
		//libcurl.AddHeader("Content-Type", "application/json;charset=UTF-8");

		//cJSON* jespJson = cJSON_CreateObject();
		//cJSON_AddStringToObject(jespJson, "type", "answer");
		//cJSON_AddStringToObject(jespJson, "sdp", sdp.c_str());

		////std::string jesp = cJSON_Print(jespJson);


		////Json::Value msgBody;
		////msgBody["request"] = "start";

		//cJSON* msgJson = cJSON_CreateObject();
		//cJSON_AddStringToObject(msgJson, "request", "start");

		////std::string msg = cJSON_Print(msgJson);

		////Json::Value jmessage;
		////jmessage["janus"] = "message";
		////jmessage["jsep"] = jesp;
		////jmessage["body"] = msgBody;
		////jmessage["transaction"] = genTransaction();
		//cJSON* pJsonObj = cJSON_CreateObject();
		//cJSON_AddStringToObject(pJsonObj, "janus", "message");
		//cJSON_AddItemToObject(pJsonObj, "jsep", jespJson);
		//cJSON_AddItemToObject(pJsonObj, "body", msgJson);
		//cJSON_AddStringToObject(pJsonObj, "transaction", genTransaction().c_str());

		////std::string json = jmessage.toStyledString();
		////json = json.substr(0, json.size() - 1);

		//std::string json = cJSON_Print(pJsonObj);
		//cJSON_Delete(pJsonObj);
		//ret = libcurl.Post((char*)url.c_str(), json.c_str(), json.size());
		//if (!ret)
		//	return ret;
		//const char* strret = libcurl.GetResponsPtr();

		//pJsonObj = cJSON_Parse(strret);
		//std::string retJanus = cJSON_GetStringFromObj(pJsonObj, "janus");
		//cJSON_Delete(pJsonObj);
		//if (strcmp(retJanus.c_str(), "ack") != 0) {

		//	return ret;
		//}

		//ret = true;
		return ret;
	}
	
}


bool SignalingSession::startWatch(std::string instancekey) {
	m_jcs = jcsWatch;
	bool ret = false;
	//Json::Value jmessage;
	//jmessage["request"] = "watch";
	//jmessage["id"] = std::to_string(streamId);
	//jmessage["ip"] = "192.168.1.156";
	//jmessage["port"] = "9993";

	cJSON* pJsonObj = cJSON_CreateObject();
	cJSON_AddStringToObject(pJsonObj, "request", "watch");
    cJSON_AddNumberToObject(pJsonObj, "videocodec", vmInstance.videoCodec);
	if (instancekey.size()>0)
		cJSON_AddStringToObject(pJsonObj, "key", instancekey.c_str());	
	else {
		cJSON_AddStringToObject(pJsonObj, "ip", vmInstance.serverDomainUrl.c_str());
		cJSON_AddStringToObject(pJsonObj, "port", std::to_string(vmInstance.serverDomainPort).c_str());
	}
	


	//std::string msg = cJSON_Print(pJsonObj);

	ret = sendMessage(pJsonObj); 
	return ret;
}

bool SignalingSession::stopWatch()  {

	cJSON* pJsonObj = cJSON_CreateObject();
	cJSON_AddStringToObject(pJsonObj, "request", "stop");

	bool ret = sendMessage(pJsonObj); 
	return ret;
	//std::string msgBody = "";
	//msgBody += "request=stop";

	//return sendMessage(msgBody);

}

void SignalingSession::waitWebrtcHungUP()
{
	if (!janusws->getConnectState())
		return;
	m_bwaitwebrtcStop = true;
	m_webrtcStopSem->wait();
}

void SignalingSession::ResetBaseInstance(gwecom::gweNetwork::BaseInstanceAbstract* baseInstance)
{
    m_instance = baseInstance;
}

/*class SetSessionDescriptionObserverAdapter :
	public webrtc::SetSessionDescriptionObserver {
public:
	SetSessionDescriptionObserverAdapter(std::function<void(std::string)> completionHandler);
	~SetSessionDescriptionObserverAdapter();
	void OnSuccess() override;
	void OnFailure(const std::string& error) override;

private:
	std::function<void(std::string)> completion_handler_;
};*/

