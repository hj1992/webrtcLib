//
// Created by Administrator on 2019/8/13.
//

#include "webrtcmodule.h"
#include <cstdlib>
#include <string.h>
#include "android/native_window.h"
#include "android/native_window_jni.h"
#include "httpmodelsrest.h"
#include "WebrtcClient.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include "jniHelper.h"
#include <vector>
#include <mutex>



std::vector<WebrtcClient*> g_vecWebClient;
std::mutex  g_WebClientMutex;


std::string g_strCacertFilePath;

void ReleaseALLWebClient();

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_vecWebClient.clear();
    jnigwecom::jniHelper::InitVM(vm);
    jint result;
    result = JNI_VERSION_1_4;
    LOGI("JNI_OnLoad\n");
    return result;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM* vm, void* reserved)
{
    LOGI("JNI_OnUnload\n");
    ReleaseALLWebClient();
  
}

JNIEXPORT
std::string JNICALL jstringTostring(JNIEnv * env, jstring jstr)
{
    jnigwecom::jniHelper stJHelper;
    std::string strResult;
    stJHelper.ConvertJavaStringToUTF8(jstr, strResult);
    return strResult;
}

JNIEXPORT
int JNICALL ConvertJByteaArrayToChars(JNIEnv *env, jbyteArray jSrcData, char*& pDstData)
{
    jbyte *bytes;
    bytes = env->GetByteArrayElements(jSrcData, 0);
    int nDataLen = env->GetArrayLength(jSrcData);
    if (nDataLen > 0)
    {
        pDstData = new char[nDataLen + 1];
        memset(pDstData, 0, nDataLen + 1);
        memcpy(pDstData, bytes, nDataLen);
        pDstData[nDataLen] = 0;
    }
    env->ReleaseByteArrayElements(jSrcData, bytes, 0);
    return nDataLen;
}


JNIEXPORT
bool JNICALL CheckWebClient(jlong jhandle, WebrtcClient*& pWebrtcClient)
{
    bool bRes = false;
    if (jhandle != 0)
    {
        std::vector<WebrtcClient*>::iterator iter = std::find(g_vecWebClient.begin(), g_vecWebClient.end(), (WebrtcClient*)jhandle);
        if (iter != g_vecWebClient.end())
        {
            pWebrtcClient = *iter;
            bRes = true;
          
        }
    }
    LOGI("CheckWebClient return_%d\n", bRes);
    return bRes;
}

JNIEXPORT
bool JNICALL SaveWebClient(WebrtcClient* pWebClient)
{
    bool bRes = false;
    std::vector<WebrtcClient*>::iterator iter = std::find(g_vecWebClient.begin(), g_vecWebClient.end(), (WebrtcClient*)pWebClient);
    if (iter == g_vecWebClient.end())
    {
        std::lock_guard<std::mutex> lck(g_WebClientMutex);
        g_vecWebClient.push_back(pWebClient);
        bRes = true;
        LOGI("SaveWebClient return_%d\n", bRes);
    }
    return bRes;
}

JNIEXPORT
bool JNICALL ReleaseWebClient(WebrtcClient*& pWebClient)
{
    bool bRes = false;
   
    std::vector<WebrtcClient*>::iterator iter = std::find(g_vecWebClient.begin(), g_vecWebClient.end(), (WebrtcClient*)pWebClient);
    if (iter != g_vecWebClient.end())
    {
        if (pWebClient)
        {
            delete pWebClient;
            pWebClient = nullptr;
        }
        std::lock_guard<std::mutex> lck(g_WebClientMutex);
        g_vecWebClient.erase(iter);
        bRes = true;
    }
    LOGI("ReleaseWebClient return_%d_Size:%d\n", bRes, g_vecWebClient.size());
    return bRes;
}

JNIEXPORT
void JNICALL ReleaseALLWebClient()
{
  
    for (std::vector<WebrtcClient*>::iterator iter = g_vecWebClient.begin(); iter != g_vecWebClient.end(); ++iter)
    {
        std::lock_guard<std::mutex> lck(g_WebClientMutex);
        WebrtcClient* pWebClient = (*iter);
        if (pWebClient)
        {
            pWebClient->StopKeepAlive();
            pWebClient->ResetBaseInstance(NULL);

            delete pWebClient;
            pWebClient = nullptr;
        }
        iter = g_vecWebClient.erase(iter);
    }
    LOGI("ReleaseALLWebClient \n");
}

extern "C"
JNIEXPORT
jlong JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_CreateClient(
    JNIEnv *env, jobject,
    jstring jsonappInstanceData, jlong baseInstance, jstring strCacertFilePath)
{
    gwecom::app::VappInstanceData  vmData;
    std::string strappInstanceData = jstringTostring(env, jsonappInstanceData);
    g_strCacertFilePath = jstringTostring(env, strCacertFilePath);

    WebrtcClient* pWebClient = new WebrtcClient(strappInstanceData, (gwecom::gweNetwork::BaseInstanceAbstract*)baseInstance);

    if (pWebClient)
    {
        SaveWebClient(pWebClient);
        return (jlong)pWebClient;
    }
    return 0;
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_InitSignaling(
    JNIEnv *env, jobject,jlong clientHandle)
{

    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient)
        && pWebClient)
    {
        pWebClient->initSignaling();
    }
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_DestroyClient(
    JNIEnv *env, jobject,jlong clientHandle)
{
    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient) 
        && pWebClient)
    {
        if (pWebClient)
            pWebClient->StopKeepAlive();

        if (pWebClient)
        {
            pWebClient->ResetBaseInstance(NULL);
        }
        ReleaseWebClient(pWebClient);
    }
   

}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_SetClientCallBack(JNIEnv *env, jobject, jlong clientHandle, jobject callback)
{
    jnigwecom::jniHelper::RegCallBack(env, jnigwecom::CALL_TO_WEBRTCCLIENT|(clientHandle&0x00), callback);
    LOGI("SetClientCallBack \n");
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_SendAnswer(JNIEnv *env, jobject, jlong clientHandle, jstring sdp)
{
    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient)
        && pWebClient)
    {
        pWebClient->SendAnswer(jstringTostring(env, sdp));
    }
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_SendTrickle(JNIEnv *env, jobject, jlong clientHandle, jstring candidateInfo)
{
    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient)
        && pWebClient)
    {
        pWebClient->SendTrickle(jstringTostring(env, candidateInfo));
    }
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_StartKeepAlive(JNIEnv *env, jobject, jlong clientHandle)
{
    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient)
        && pWebClient)
    {
        pWebClient->StartKeepAlive();
    }
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_StopKeepAlive(JNIEnv *env, jobject, jlong clientHandle)
{
    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient)
        && pWebClient)
    {
        pWebClient->StopKeepAlive();
    }
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_RecieveVideoStream(JNIEnv *env, jobject, jlong clientHandle, jbyteArray pData, jint nDataLen)
{
    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient)
        && pWebClient)
    {
        char *pDataBuf = nullptr;
        int nDatalen = ConvertJByteaArrayToChars(env, pData, pDataBuf);
        if (pWebClient && nDatalen > 0)
        {
            pWebClient->RecieveVideoStream(pDataBuf, nDatalen);
        }
        if (pDataBuf)
        {
            delete[] pDataBuf;
            pDataBuf = NULL;
        }
    }
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_RecieveAudioStream(JNIEnv *env, jobject, jlong clientHandle, jbyteArray pData, jint nDataLen)
{
WebrtcClient* pWebClient = nullptr;
if (CheckWebClient(clientHandle, pWebClient)
&& pWebClient)
{
char *pDataBuf = nullptr;
int nDatalen = ConvertJByteaArrayToChars(env, pData, pDataBuf);
if (pWebClient && nDatalen > 0)
{
pWebClient->RecieveAudioStream(pDataBuf, nDatalen);
}
if (pDataBuf)
{
delete[] pDataBuf;
pDataBuf = NULL;
}
}
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_WebrtcMessage(JNIEnv *env, jobject, jlong clientHandle, jbyteArray pData, jint nDataLen, jboolean isbinary)
{
    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient)
        && pWebClient)
    {
        char *pDataBuf = nullptr;
        int nDatalen = ConvertJByteaArrayToChars(env, pData, pDataBuf);
        if (pWebClient && nDatalen > 0)
        {
            pWebClient->WebrtcMessage(pDataBuf, nDatalen, isbinary==JNI_TRUE);
        }
        if (pDataBuf)
        {
            delete[] pDataBuf;
            pDataBuf = NULL;
        }
    }
}


extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_RecieveRtt(JNIEnv *env, jobject, jlong clientHandle, jlong lrttTime)
{
    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient)
        && pWebClient)
    {
        pWebClient->RecieveRtt(lrttTime);
    }
}

extern "C"
JNIEXPORT
void JNICALL Java_com_gwecom_webrtcmodule_NativeClient_WebrtcModule_OnWebrtcError(JNIEnv *env, jobject, jlong clientHandle, jint nerror)
{
    WebrtcClient* pWebClient = nullptr;
    if (CheckWebClient(clientHandle, pWebClient)
        && pWebClient)
    {
        pWebClient->OnWebrtcError(nerror);
    }
}

