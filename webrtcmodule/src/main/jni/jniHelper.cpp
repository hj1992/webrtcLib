//
// Created by Administrator on 2019/9/9.
//

//#define ANDROID
#ifdef ANDROID

#include "jniHelper.h"
#include<pthread.h>
#include<sys/prctl.h>
#include <locale>
#include <codecvt>
#include <sstream>
#include <map>
#include "LogUtil.h"
#include <mutex>


using namespace jnigwecom;

JavaVM* jniHelper::s_jvm = nullptr;
std::mutex jniHelper::s_mtx;
jobject jniHelper::s_classLoader;
std::map<int, jobject> g_mapCallBack;
jmethodID g_class_loader_load_class_method_id = 0;
pthread_key_t jniHelper::s_threadKey=0;
static pthread_once_t once = PTHREAD_ONCE_INIT;
static std::mutex mutexlock;



//////////////////////////////////////////////////////////////////////////

jniHelper::jniHelper()
{
    m_bAttacked = false;
    m_env = nullptr;
}

jniHelper::~jniHelper()
{
    //DetachFromVM();
}

JNIEnv* jniHelper::GetEnv()
{
    if (IsEnvInitialized())
    {
        return m_env;
    }
    return nullptr;
}

void jniHelper::InitVM(JavaVM* vm)
{
    std::lock_guard<std::mutex> lck(s_mtx);
    s_jvm = vm;
}

bool jniHelper::IsVMInitialized()
{
    std::lock_guard<std::mutex> lck(s_mtx);
    return s_jvm != nullptr;
}

bool jniHelper::RegCallBack(JNIEnv *env, int nCallBackId, jobject callback)
{
    if (!IsVMInitialized())
    {
        return false;
    }
    jniHelper stJHelper;
    if (!env)
    {
        env = stJHelper.GetEnv();
    }
    std::lock_guard<std::mutex> lck(s_mtx);
    if (g_mapCallBack.count(nCallBackId) > 0)
    {
        jobject & tmpcallback= g_mapCallBack[nCallBackId];
        if (tmpcallback)
        {
            env->DeleteGlobalRef(tmpcallback);
            tmpcallback=nullptr;
        }
        g_mapCallBack.erase(nCallBackId);
    }
    if (g_mapCallBack.count(nCallBackId) == 0)
    {
        g_mapCallBack[nCallBackId] =env->NewGlobalRef(callback);
    }
    return true;
}

void jniHelper::RegCallBackClear()
{
    if (!IsVMInitialized())
    {
        return;
    }
    jniHelper stJHelper;
    JNIEnv *env= stJHelper.GetEnv();
    if (env)
    {
        std::lock_guard<std::mutex> lck(s_mtx);
        for (std::map<int, jobject>::iterator iter = g_mapCallBack.begin(); iter != g_mapCallBack.end(); ++iter)
        {
            jobject & tmpcallback =(iter->second);
            if (tmpcallback
                )
            {
                env->DeleteGlobalRef(tmpcallback);
                tmpcallback = nullptr;
            }
        }
        g_mapCallBack.clear();
    }
}

jobject jniHelper::GetCallBack(int nCallBackId)
{
    std::lock_guard<std::mutex> lck(s_mtx);
    if (g_mapCallBack.count(nCallBackId) > 0)
    {
        return  g_mapCallBack[nCallBackId];
    }
    return nullptr;
}

JNIEnv* jniHelper::AttachCurrentThread()
{
    if (!IsVMInitialized())
    {
        return  nullptr;
    }
    JNIEnv* env = nullptr;
    jint ret = s_jvm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_4);
    switch (ret)
    {
    case JNI_OK:
    {
        return env;
    }
    break;
    case JNI_EDETACHED:
    {
        JavaVMAttachArgs args;
        args.version = JNI_VERSION_1_4;
        args.group = nullptr;

        char thread_name[16] = { '\0' };
        int err = prctl(PR_GET_NAME, thread_name);
        if (err < 0)
        {
            args.name = nullptr;
        }
        else
        {
            args.name = thread_name;
        }
        if (s_jvm->AttachCurrentThread(&env, &args) !=JNI_OK)
        {
            return nullptr;
        }
        else
        {
            m_bAttacked= true;
            std::lock_guard<std::mutex> lck(mutexlock);
            pthread_setspecific(s_threadKey, env);
            return env;
        }
    }
    break;
    case JNI_EVERSION:
    default:
        return nullptr;
    }
    return env;
}

void jniHelper::DetachFromVM()
{
    if (s_jvm && m_bAttacked)
    {
        s_jvm->DetachCurrentThread();
    }
    m_env = nullptr;
}

bool jniHelper::HasException()
{
    if (!IsEnvInitialized())
    {
        return false;
    }
    return m_env->ExceptionCheck() != JNI_FALSE;
}

bool jniHelper::ClearException()
{
    if (!IsEnvInitialized())
    {
        return false;
    }
    if (!HasException())
        return false;
    m_env->ExceptionDescribe();
    m_env->ExceptionClear();
    return true;
}

void jniHelper::CheckException()
{
    if (!IsEnvInitialized())
    {
        return;
    }
    if (!HasException())
        return;

    jthrowable java_throwable = m_env->ExceptionOccurred();
    if (java_throwable)
    {
        m_env->ExceptionDescribe();
        m_env->ExceptionClear();

        m_env->DeleteLocalRef(java_throwable);
    }

}

void jniHelper::ConvertJavaStringToUTF8(jstring str, std::string& result)
{
    if (!IsEnvInitialized())
    {
        return;
    }
    if (!str)
    {
        result.clear();
        return;
    }
    const jsize length = m_env->GetStringLength(str);
    if (!length)
    {
        result.clear();
        return;
    }
    std::stringstream ss;
    const  char* pChars = m_env->GetStringUTFChars(str, NULL);

    if (pChars)
    {
        std::stringstream ss;
        ss.write(reinterpret_cast<const char *>(pChars), strlen(pChars));
        result = ss.str();
    }
    m_env->ReleaseStringUTFChars(str, pChars);
    CheckException();
}

void jniHelper::ConvertUTF8ToJavaString(std::string str, jstring & result)
{
    if (!IsEnvInitialized())
    {
        return;
    }
    result = m_env->NewStringUTF(reinterpret_cast<const char*>(str.c_str()));
    CheckException();
}

void jniHelper::detach_env(void *data)
{
    if (s_jvm)
    {
        s_jvm->DetachCurrentThread();
    }
}

void jniHelper::create_pthread_key()
{
    pthread_key_create(&s_threadKey, jniHelper::detach_env);
}


bool jniHelper::IsEnvInitialized()
{
    if(!m_env && true)
    {
        std::lock_guard<std::mutex> lck(mutexlock);
       pthread_once(&once, jniHelper::create_pthread_key);
    }
    if(!m_env)
    {
        std::lock_guard<std::mutex> lck(mutexlock);
        m_env = (JNIEnv *)pthread_getspecific(s_threadKey);
    }
    if (!m_env)
    {
        m_env = AttachCurrentThread();
    }

    return m_env != nullptr;
}


#endif //ANDROID

