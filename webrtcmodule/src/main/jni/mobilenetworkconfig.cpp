//
// Created by gwe on 2019/4/9.
//

#include "mobilenetworkconfig.h"
#include <fstream>
#include <iostream>
#include <sstream>


#ifdef ANDROID

using namespace jnigwecom;


jstring ConverToJstring(const char* pStrData, int nStrDataLen)
{
#if 0
    JNIEnv *env;
    if (g_jvm->AttachCurrentThread(&env, NULL) != JNI_OK)
    {
        return jstring("");
    }
    jclass strClass = (env)->FindClass("java/lang/String");
    jmethodID ctorID = (env)->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jbyteArray bytes = (env)->NewByteArray(nStrDataLen);
    (env)->SetByteArrayRegion(bytes, 0, nStrDataLen, (jbyte*)pStrData);
    jstring encoding = (env)->NewStringUTF("GB2312");
    return (jstring)(env)->NewObject(strClass, ctorID, bytes, encoding);
#else
    std::ostringstream oss;
    oss.write(pStrData, nStrDataLen);
    jnigwecom::jniHelper stJHelper;
    jstring encoding;
    stJHelper.ConvertUTF8ToJavaString(oss.str(), encoding);
    return encoding;
#endif

}



int CallBack_InitRTCModule(std::string strappInstanceData, void* pBaseInstance)
{
    JNIEnv *env = NULL;
    jclass cls = NULL;
    jmethodID mid = NULL;
    jint nRes=-1;

    jnigwecom::jniHelper stJHelper;
    env = stJHelper.GetEnv();
    jobject jCallBackObj = stJHelper.GetCallBack(CALL_TO_WEBRTCCLIENT);
    if (env
        && jCallBackObj)
    {
        //找到对应的类
        cls = env->GetObjectClass(jCallBackObj);
        if (cls == NULL)
        {
            return nRes;
        }
        mid = env->GetMethodID(cls,
            "InitRTCModule", "(Ljava/lang/String;J)I");
        if (mid)
        {
            jstring jstrappInstanceData;
            stJHelper.ConvertUTF8ToJavaString(strappInstanceData, jstrappInstanceData);
            nRes=env->CallIntMethod(jCallBackObj, mid, jstrappInstanceData, (jlong)pBaseInstance);

            if (stJHelper.ClearException())
            {
                LOGI("ClearException has Exception");
            }
            if (jstrappInstanceData)
            {
                env->DeleteLocalRef(jstrappInstanceData);
                jstrappInstanceData = nullptr;
            }
        }
        if (cls)
        {
            env->DeleteLocalRef(cls);
            cls = nullptr;
        }
    }
    else
        {
         LOGI("not found jCallBackObj");
        }
    jCallBackObj = nullptr;
    env = nullptr;
    return  nRes;
}

void CallBack_ChannelMsg(std::string strData)
{
    JNIEnv *env = NULL;
    jclass cls = NULL;
    jmethodID mid = NULL;

    jnigwecom::jniHelper stJHelper;
    env = stJHelper.GetEnv();
    jobject jCallBackObj = stJHelper.GetCallBack(CALL_TO_WEBRTCCLIENT);
    if (env
        && jCallBackObj)
    {
        //找到对应的类
        cls = env->GetObjectClass(jCallBackObj);
        if (cls == NULL)
        {
            return;
        }
        mid = env->GetMethodID(cls,
            "SendData", "(Ljava/lang/String;)V");
        if (mid)
        {
            jstring jstrData;
            stJHelper.ConvertUTF8ToJavaString(strData, jstrData);
            env->CallVoidMethod(jCallBackObj, mid, jstrData);

            if (stJHelper.ClearException())
            {
                LOGI("ClearException has Exception");
            }
            if (jstrData)
            {
                env->DeleteLocalRef(jstrData);
                jstrData = nullptr;
            }
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

void CallBack_UninitializeRTCModule()
{

    JNIEnv *env = NULL;
    jclass cls = NULL;
    jmethodID mid = NULL;

    jnigwecom::jniHelper stJHelper;
    env = stJHelper.GetEnv();
    jobject jCallBackObj = stJHelper.GetCallBack(CALL_TO_WEBRTCCLIENT);
    if (env
        && jCallBackObj)
    {
        //找到对应的类
        cls = env->GetObjectClass(jCallBackObj);
        if (cls == NULL)
        {
            return;
        }
        mid = env->GetMethodID(cls,
            "Uninitialize", "()Z");
        if (mid)
        {

            env->CallBooleanMethod(jCallBackObj, mid);

            if (stJHelper.ClearException())
            {
                LOGI("ClearException has Exception");
            }

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


#endif

