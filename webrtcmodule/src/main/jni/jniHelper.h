#ifndef JNIDEMO_JNIHELPER_H
#define JNIDEMO_JNIHELPER_H
#ifdef ANDROID

#include <jni.h>
#include <string>
#include <atomic>
#include "LogUtil.h"
#include <mutex>
#include <pthread.h>



namespace jnigwecom
{
    enum CallBackType
    {
        CALL_TO_GETSIZE = 0x00,
        CALL_TO_GETQUALITY = 0x01,
        CALL_TO_WEBRTCCLIENT = 0x02,
    };

    class jniHelper
    {
    public:
        static void InitVM(JavaVM* vm);
        static bool IsVMInitialized();
        static bool RegCallBack(JNIEnv *env,int nCallBackId, jobject callback);
        static void RegCallBackClear();
        static jobject GetCallBack(int nCallBackId);
    public:
        jniHelper();
        virtual ~jniHelper();
        JNIEnv* GetEnv();
    private:
        JNIEnv* AttachCurrentThread();
        void DetachFromVM();
    public:
        bool HasException();
        bool ClearException();
        void CheckException();
    public:
        void ConvertJavaStringToUTF8(jstring str, std::string& result);
        void ConvertUTF8ToJavaString(std::string str, jstring & result);
    public:
        //static void InitReplacementClassLoader(jobject & class_loader);
         static void detach_env(void *data);
        static void create_pthread_key();
    private:
        bool IsEnvInitialized();
    private:
        static JavaVM* s_jvm;
        static std::mutex s_mtx;
        static jobject s_classLoader;
        static pthread_key_t s_threadKey;
        JNIEnv* m_env;
        bool  m_bAttacked;
    };
}

#endif //ANDROID
#endif //JNIDEMO_JNIHELPER_H
