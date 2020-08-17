//
// Created by gwe on 2019/4/9.
//

#ifndef ANDROIDCLIENT_MOBILENETWORKCONFIG_H
#define ANDROIDCLIENT_MOBILENETWORKCONFIG_H

#include <string>
#ifdef ANDROID
#include <jni.h>
#include "jniHelper.h"
#include "LogUtil.h"

extern std::string wholeinstandid;
extern std::string wholeuuid;
extern std::string wholeconnectuuid;

#endif
#define FORGAME 1
#define FORNOBACKGROUND 0

#if FORGAME  //公版 游戏项目
#define UseMulityChannel 1		// 1: open, 0: close
#define PC 0
#define NetDetect 1				// 1: open, 0: close
#define GetEncodeBitrate 1		// 1: send request, 0: not send
#elif FORNOBACKGROUND //直连版本
#define UseMulityChannel 0		// 1: open, 0: close
#define PC 0
#define NetDetect 0			// 1: open, 0: close
#define GetEncodeBitrate 0		// 1: send request, 0: not send
#elif FORTOB       //tob项目
#define UseMulityChannel 0		// 1: open, 0: close
#define PC 0
#define NetDetect 0			// 1: open, 0: close
#define GetEncodeBitrate 0		// 1: send request, 0: not send
#endif

enum NetWorkError{
    nweNoError =0,
    nweScktError = -1,
    newHandShakeError = -2,
};

/*
#define ANDROID
#define DEBUG_WEBRTC*/

#ifdef ANDROID

jstring ConverToJstring(const char* pStrData, int nStrDataLen);

#ifdef DEBUG_WEBRTC 
void CallBack_InitRTCModule(std::string strappInstanceData, void* pBaseInstance);
void CallBack_ChannelMsg(std::string strData);
void CallBack_UninitializeRTCModule();
#endif

//void WriteLogNetWork(const char* format, ...);
#define WriteLogNetWork(format, ...) LOGI(format, ##__VA_ARGS__)
#elif IOS
#define WriteLogNetWork(format, ...) printf(format,##__VA_ARGS__)
#endif

#endif //ANDROIDCLIENT_MOBILENETWORKCONFIG_H
