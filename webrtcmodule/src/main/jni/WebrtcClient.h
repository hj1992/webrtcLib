#ifndef WEBRTCCLIENT_H
#define WEBRTCCLIENT_H

#include <android/native_window.h>
#include "signalingsession.h" 
#include "baseinstanceabstract.h"
#include <iostream>
#include <thread>
#include <memory>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <mutex>
#include <functional>

extern JavaVM *g_ClientCallBackJvm;
extern jobject g_ClientCallBackObj;
extern JNIEnv *g_ClientCallBackenv;
extern jclass g_ClientCallBackCls;

extern JavaVM *g_JavaVM;

class WebrtcClient :
    public std::enable_shared_from_this<WebrtcClient>
{
public:
    WebrtcClient(std::string  strVmData, gwecom::gweNetwork::BaseInstanceAbstract* baseInstance);
    WebrtcClient(const gwecom::app::VappInstanceData & vmData, gwecom::gweNetwork::BaseInstanceAbstract* baseInstance);
    virtual ~WebrtcClient();
    void initSignaling();
    void initSignaling_VMCallBack();
    //void ConnectJanus();
    void onRecieveSDP(std::string& sdp);
    void SendAnswer(std::string sdp);
    void SendTrickle(std::string sdp);
    void StartKeepAlive();
    void StopKeepAlive();
    void RecieveVideoStream(char* pData, int nDataLen);
    void RecieveAudioStream(char* pData, int nDataLen);
    void WebrtcMessage(char* pData, int nDataLen,bool isbinary);
    void ResetBaseInstance(gwecom::gweNetwork::BaseInstanceAbstract* baseInstance);
    void RecieveRtt(long lrtttime);
    void OnWebrtcError(int nError);
public:
    bool longPolling();
    static void sendAlive(void* user);
private:
    void CallBack_InitPeerConnection(std::string strUsername, std::string strCredential, std::string strTurnSrvUrl, bool bFinish);
    void CallBack_InitPeerConnection(std::string striceserverinfo);
    void CallBack_ConnectJanus(std::string strSdp);
private:
    gwecom::app::VappInstanceData  _vmInstance;
    SignalingSession* _signaling;
    gwecom::gweNetwork::BaseInstanceAbstract* m_instance;
    bool _active;
    std::string _remoteSdp;
    ATimer<>* kaliveTimer;
};

#endif // WEBRTCCLIENT_H
