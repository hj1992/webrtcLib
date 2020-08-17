#include "WebrtcClient.h" 

#include "cJSON.h"
#include <string>
#include <algorithm>
using namespace std;


WebrtcClient::WebrtcClient(std::string  strVmData, gwecom::gweNetwork::BaseInstanceAbstract* baseInstance)
    :m_instance(baseInstance)
{
    _signaling = nullptr;
    kaliveTimer = new ATimer<>(this);
    kaliveTimer->bind(WebrtcClient::sendAlive);
    if (!strVmData.empty())
    {
        cJSON* pJsonObj = cJSON_Parse(strVmData.c_str());
        if (pJsonObj)
        {
            cJSON* pJsonItem = cJSON_GetObjectItem(pJsonObj, "bUseSSL");
            if (pJsonItem)
                _vmInstance.bUseSSL = pJsonItem->valueint;
             

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "frameWidth");
            if (pJsonItem)
                _vmInstance.frameWidth = pJsonItem->valueint;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "frameHeight");
            if (pJsonItem)
                _vmInstance.frameHeight = pJsonItem->valueint;


            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "useWebrtc");
            if (pJsonItem)
                _vmInstance.useWebrtc = pJsonItem->valueint;


            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "useTurnServer");
            if (pJsonItem)
                _vmInstance.useTurnServer = pJsonItem->valueint;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "turnServerIP");
            if (pJsonItem)
                _vmInstance.turnServerIP = pJsonItem->valuestring;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "turnServerPort");
            if (pJsonItem)
                _vmInstance.turnServerPort = pJsonItem->valueint;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "instanceKey");
            if (pJsonItem)
                _vmInstance.instanceKey = pJsonItem->valuestring;


            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "serverDomainUrl");
            if (pJsonItem)
                _vmInstance.serverDomainUrl = pJsonItem->valuestring;


            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "serverDomainPort");
            if (pJsonItem)
                _vmInstance.serverDomainPort = pJsonItem->valueint;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "controlImgUrl");
            if (pJsonItem)
                _vmInstance.controlImgUrl = pJsonItem->valuestring;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "screenDirect");
            if (pJsonItem)
                _vmInstance.screenDirect = pJsonItem->valueint;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "connectType");
            if (pJsonItem)
                _vmInstance.connectType = pJsonItem->valueint;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "accountType");
            if (pJsonItem)
                _vmInstance.accountType = pJsonItem->valueint;


            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "janusIP");
            if (pJsonItem)
                _vmInstance.janusIP = pJsonItem->valuestring;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "janusPort");
            if (pJsonItem)
                _vmInstance.janusPort = pJsonItem->valueint;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "iceServerInfo");
            if (pJsonItem)
                _vmInstance.iceServerInfo = pJsonItem->valuestring;

            pJsonItem = nullptr;
            pJsonItem = cJSON_GetObjectItem(pJsonObj, "videoCodec");
            if (pJsonItem)
                _vmInstance.videoCodec = pJsonItem->valueint;

        }
        if (pJsonObj)
            cJSON_Delete(pJsonObj);
    }
}

WebrtcClient::WebrtcClient(const gwecom::app::VappInstanceData  &vmData, gwecom::gweNetwork::BaseInstanceAbstract* baseInstance)
    :_vmInstance(vmData),
    m_instance(baseInstance)
{
    _signaling = nullptr;
    kaliveTimer = new ATimer<>(this);
    kaliveTimer->bind(WebrtcClient::sendAlive);
}

WebrtcClient::~WebrtcClient()
{

    if (kaliveTimer)
    {
        kaliveTimer->stop();

        delete kaliveTimer;
        kaliveTimer = NULL;
    }
    if (_signaling)
    {
        _signaling->stopWatch();

        delete _signaling;
        _signaling = NULL;
    }
    if (m_instance)
    {
        m_instance = NULL;
    }
    

}
void WebrtcClient::initSignaling()
{
    _active = true;
    std::thread([this]
    {
        initSignaling_VMCallBack();
        if (_signaling == nullptr)
        {
            _signaling = new SignalingSession(this, _vmInstance, m_instance);
        }
    }).detach();
}

void WebrtcClient::initSignaling_VMCallBack()
{

    std::string striceServerInfo = _vmInstance.iceServerInfo;
    striceServerInfo.erase(std::remove(striceServerInfo.begin(), striceServerInfo.end(), '\\'), striceServerInfo.end());
    const char* iceServerInfo = striceServerInfo.c_str();
    cJSON* serverInfo = cJSON_Parse(iceServerInfo);
    if (!serverInfo)
    {
        return;
    }
   
#if 0
    CallBack_InitPeerConnection(striceServerInfo);
#else
   //  CallBack_InitPeerConnection("gwe", "gwe", "turn:182.150.22.249:3479");
    int iArrSize = cJSON_GetArraySize(serverInfo);
    if(iArrSize<=0)
    {
        CallBack_InitPeerConnection("", "", "", true);
    }

    for (int i = 0; i < iArrSize; ++i)
    {
        cJSON* pEachJsonData = cJSON_GetArrayItem(serverInfo, i);
        if (pEachJsonData)
        {
            char* user;
            char* pwd;
            char* url;
            user = cJSON_GetStringFromObj(pEachJsonData, "username");
            pwd = cJSON_GetStringFromObj(pEachJsonData, "credential");
            url = cJSON_GetStringFromObj(pEachJsonData, "url");
            std::stringstream turn;
            if (url)
                turn << url;
            std::stringstream ssUser;
            if (user)
                ssUser << user;
            std::stringstream ssPwd;
            if (pwd)
                ssPwd << pwd;

            string strUser = ssUser.str();
            string strPwd = ssPwd.str();
            string strTurn = turn.str();

            bool  bFinished=(i+1==iArrSize);
           CallBack_InitPeerConnection(strUser, strPwd, strTurn,bFinished);

        }
    }
#endif
    if (serverInfo)
    {
        cJSON_Delete(serverInfo);
        serverInfo = NULL;
    }
}

void WebrtcClient::onRecieveSDP(std::string& sdp)
{
    _remoteSdp = sdp;
    CallBack_ConnectJanus(_remoteSdp);
}
void WebrtcClient::SendAnswer(std::string sdp)
{
    if (_signaling&& _active)
        _signaling->sendAnswer(sdp);
}

void WebrtcClient::SendTrickle(std::string sdp)
{
    if (_signaling&& _active)
    {
        cJSON* pJsonObj = cJSON_Parse(sdp.c_str());
        _signaling->sendTrickle(pJsonObj);
    }

}

bool WebrtcClient::longPolling()
{
    if (_signaling != nullptr)
    {
        return    _signaling->keepAlive();
    }
    return true;
}

void WebrtcClient::sendAlive(void* user)
{
    WebrtcClient* pThis = (WebrtcClient*)user;
    if (pThis)
    {
        pThis->longPolling();
    }

}

void WebrtcClient::StartKeepAlive()
{
    if (_signaling
    && _active
    && kaliveTimer
    && !kaliveTimer->isActive())
    {
        kaliveTimer->start(10000);
        LOGI("StartKeepAlive\n");
    }
    if (m_instance)
        m_instance->onConnected(0);
}

void WebrtcClient::StopKeepAlive()
{
    if (kaliveTimer&& kaliveTimer->isActive())
    {
        kaliveTimer->stop();
        LOGI("StopKeepAlive\n");
    }
}

void WebrtcClient::RecieveVideoStream(char* pData, int nDataLen)
{
#if 0
    LOGI("收到视频数据_%d\n",nDataLen);
    static FILE *qpcmfp = fopen("/sdcard/RecieveVideoStream.h264", "wb+");
    fwrite(pData, sizeof(char), nDataLen, qpcmfp);
    fflush(qpcmfp);
#endif
    if (m_instance)
        m_instance->onRecieveVideoStream(pData, nDataLen);
}

void WebrtcClient::RecieveAudioStream(char* pData, int nDataLen)
{
    if (m_instance)
        m_instance->onRecieveAudioStream(pData, nDataLen);
}

void WebrtcClient::WebrtcMessage(char* pData, int nDataLen, bool isbinary)
{
    LOGI("WebrtcMessage_%d\n",nDataLen);
    if (m_instance)
        m_instance->onWebrtcMessage(pData, nDataLen,isbinary);
}

void WebrtcClient::ResetBaseInstance(gwecom::gweNetwork::BaseInstanceAbstract* baseInstance)
{
    if (_signaling)
    {
        _signaling->ResetBaseInstance(baseInstance);
    }
    m_instance = baseInstance;
}

void WebrtcClient::RecieveRtt(long lrtttime)
{
    if (m_instance)
        m_instance->onRecieveRtt(lrtttime);
}

void WebrtcClient::OnWebrtcError(int nError)
{
    if (m_instance)
        m_instance->onWebrtcError(nError);
}

void WebrtcClient::CallBack_InitPeerConnection(std::string strUsername, std::string strCredential, std::string strTurnSrvUrl,bool bFinish)
{
    JNIEnv *env = nullptr;
    jclass cls = nullptr;
    jmethodID mid = nullptr;

    jnigwecom::jniHelper stJHelper;

    LOGI("strUsername:%s_%s_%s_init\n", strUsername.c_str(), strCredential.c_str(), strTurnSrvUrl.c_str());

    env = stJHelper.GetEnv();
    jobject jCallBackObj = stJHelper.GetCallBack(jnigwecom::CALL_TO_WEBRTCCLIENT | ((long)this& 0x00));

    if (env
        && jCallBackObj)
    {

        //找到对应的类
        cls = env->GetObjectClass(jCallBackObj);
        if (cls == nullptr)
        {
            LOGI("strUsername:%s_%s_%s_GetObjectClass Failed\n", strUsername.c_str(), strCredential.c_str(), strTurnSrvUrl.c_str());
            return;
        }
    //再获得类中的方法
#if 1
    mid = env->GetMethodID(cls,
        "InitPeerConnection",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;IZ)V");
    if (mid == NULL)
    {
        LOGI("strUsername:%s_%s_%s_GetMethodID Failed\n", strUsername.c_str(), strCredential.c_str(), strTurnSrvUrl.c_str());
        return;
    }
    LOGI("strUsername:%s_%s_%s_%d_%d\n", strUsername.c_str(), strCredential.c_str(), strTurnSrvUrl.c_str(), _vmInstance.useWebrtc,bFinish);

    jstring jstrUsername=nullptr;
   // if (!strUsername.empty())
    {
        stJHelper.ConvertUTF8ToJavaString(strUsername, jstrUsername);
    }

    jstring jstrCredential=nullptr;
   // if (!strCredential.empty())
    {
        stJHelper.ConvertUTF8ToJavaString(strCredential, jstrCredential);
    }
   
    jstring jstrTurnSrvUrl=nullptr;
  //  if (!strTurnSrvUrl.empty())
    {
        stJHelper.ConvertUTF8ToJavaString(strTurnSrvUrl, jstrTurnSrvUrl);
    }
    env->CallVoidMethod(jCallBackObj, mid, jstrUsername, jstrCredential, jstrTurnSrvUrl, _vmInstance.useWebrtc, bFinish);
#else
    mid = env->GetMethodID(cls,
        "InitPeerConnection",
        "()V");
    if (mid == NULL)
    {
        return;
    }
    env->CallVoidMethod(g_ClientCallBackObj, mid);
#endif

    if (stJHelper.ClearException())
    {
        LOGI("ClearException has Exception");
    }

#if 1
    if (jstrUsername)
    {
        env->DeleteLocalRef(jstrUsername);
        jstrUsername = nullptr;
    }
    if (jstrCredential)
    {
        env->DeleteLocalRef(jstrCredential);
        jstrUsername = nullptr;
    }
    if (jstrTurnSrvUrl)
    {
        env->DeleteLocalRef(jstrTurnSrvUrl);
        jstrTurnSrvUrl = nullptr;
    }
#endif
    if (cls)
    {
        env->DeleteLocalRef(cls);
        cls = nullptr;
    }

    }
    jCallBackObj = nullptr;
    env = nullptr;
}

void WebrtcClient::CallBack_InitPeerConnection(std::string striceserverinfo)
{
    JNIEnv *env = nullptr;
    jclass cls = nullptr;
    jmethodID mid = nullptr;

    jnigwecom::jniHelper stJHelper;

    env = stJHelper.GetEnv();
    jobject jCallBackObj = stJHelper.GetCallBack(jnigwecom::CALL_TO_WEBRTCCLIENT | ((long)this & 0x00));

    if (env
        && jCallBackObj)
    {

        //找到对应的类
        cls = env->GetObjectClass(jCallBackObj);
        if (cls == nullptr)
        {
            return;
        }
        //再获得类中的方法
        mid = env->GetMethodID(cls,
            "InitPeerConnection",
            "(Ljava/lang/String;)V");
        if (mid == NULL)
        {
            return;
        }
        LOGI("striceserverinfo:%s\n", striceserverinfo.c_str());
        jstring jstriceserverinfo;
        stJHelper.ConvertUTF8ToJavaString(striceserverinfo, jstriceserverinfo);
        env->CallVoidMethod(jCallBackObj, mid, jstriceserverinfo);
        if (stJHelper.ClearException())
        {
            LOGI("ClearException has Exception");
        }

        if (jstriceserverinfo)
        {
            env->DeleteLocalRef(jstriceserverinfo);
            jstriceserverinfo = nullptr;
        }
        if (cls)
        {
            env->DeleteLocalRef(cls);
            cls = nullptr;
        }

    }
    jCallBackObj = nullptr;
    env = nullptr;
}

void WebrtcClient::CallBack_ConnectJanus(std::string strSdp)
{
    JNIEnv *env= nullptr;
    jclass cls= nullptr;
    jmethodID mid= nullptr;

    jnigwecom::jniHelper stJHelper;

    env = stJHelper.GetEnv();
    jobject jCallBackObj = stJHelper.GetCallBack(jnigwecom::CALL_TO_WEBRTCCLIENT | ((long)this & 0x00));

    if (env
        && jCallBackObj)
    {

        //找到对应的类
        cls = env->GetObjectClass(jCallBackObj);
        if (cls == nullptr)
        {
            return;
        }
    //再获得类中的方法
    mid = env->GetMethodID(cls,
        "ConnectJanus", "(Ljava/lang/String;)V");
    if (mid == nullptr)
    {
        return;
    }
    jstring jstrSdp;
    stJHelper.ConvertUTF8ToJavaString(strSdp, jstrSdp);

    env->CallVoidMethod(jCallBackObj, mid, jstrSdp);

    if (stJHelper.ClearException())
    {
        LOGI("ClearException has Exception");
    }
    if (jstrSdp)
    {
        env->DeleteLocalRef(jstrSdp);
        jstrSdp = nullptr;
    }
    if (cls)
    {
        env->DeleteLocalRef(cls);
        cls = nullptr;
    }

    }
    jCallBackObj = nullptr;
    env = nullptr;
}
