#ifndef BASE_INSTANCE_ABSTRACT_H
#define BASE_INSTANCE_ABSTRACT_H
#ifdef _WIN32
class SMp4WriterInterface;
#endif
#include <string>

namespace gwecom {
	namespace gweNetwork {
		class BaseInstanceAbstract
		{
		public:
		//BaseInstanceAbstract();
        virtual	~BaseInstanceAbstract() {};


			virtual void setInited(bool inited) = 0;
			//#ifdef _WIN32
			virtual void onWebrtcMessage(char* data, int datalen,bool isbinary) = 0;

			//webrtc error begin
			virtual void onJanusWSConnected() = 0; //janus web socket connect success
			virtual void onJanusWSError() = 0;     //janus web socket error
			virtual void onJanusWebrtcUP() = 0;         //janus Webrtc Success  
			virtual void onJanusRecieveError(std::string& error) = 0;//janus return data errors 
#if defined(ANDROID)
            virtual void onWebrtcError(int nerror) = 0;//webrtc return errors 
#endif
			//webrtc error end
           //#endif

			virtual void onConnected(int channeltype) = 0;
			virtual void onError(int channeltype) = 0;
			virtual void onDNSError() = 0;
			virtual void onNetworkAnomaly(int channeltype) = 0;

			virtual void onVideoStreamParam(int codec, int iFrameRate) = 0;
#ifdef _WIN32
			virtual void onVideoStreamParam(int codec, int iFrameRate, int width, int height) = 0;
#endif
			virtual void onCloseApp() = 0;

			virtual void onConnectPushServerError() = 0;

			// 服务器到客户端
			virtual void onRecieveDesktopCount(int iCount) = 0;
			virtual void SynchronizeServerMouse() = 0;
			virtual void onRecieveVideoStream(int iAdaptervoid, void *data, int size) = 0;
			virtual void onRecieveVideoStream(void *data, int size) = 0;
			virtual void onRecieveAudioStream(void *data, int size) = 0;
			virtual void ReportRecieveVideoStream(int size) = 0;
			virtual void ReportRecieveAudioStream(int size) = 0;

			virtual void onRecieveEncodeBitrate(int iBitrate) = 0;

			virtual void onRecieveCursorVisible(void *data, int size) = 0;

#ifdef _WIN32

			virtual void onRecieveRtt() = 0;
#else
			virtual void onRecieveRtt(void *data, int size) = 0;
            virtual void onRecieveRtt(long lrttTime)=0;
#endif
			virtual void onRecieveXBoxVB(void *data, int size) = 0;

			virtual void onRecieveCursorShapeChanged(void *data, int size) = 0;

			virtual void onRecieveVideoQualityList(void *data, int size) = 0;
			virtual void onRecieveMouseTypeList(void *data, int size) = 0;

			virtual void onRecieveInputMethodTypeList(void *data, int size) = 0;

			virtual void onHandShakeError(int code, int iChannelType) = 0;
			virtual void onRecieveServerScreenMode(int iServerScreenMode) = 0;

			virtual void onRecieveRecordAudioStart() = 0;
			virtual void onRecieveRecordAudioStop() = 0;

			virtual void registerObserver(void * widget) = 0;
			virtual void registerVideoDecoder(void *decoder) = 0;
			virtual void registerAudioDecoder(void* decoder) = 0;
			#ifdef _WIN32
			virtual void registerMp4Writer(SMp4WriterInterface* pMp4Writer) = 0;
			virtual void registerVideoDecoderList(void* decoder) = 0;
			virtual bool DecodeVideo(void *data, int size, void** ppFrame) = 0;
			virtual bool RenderVideo(void* pFrame) = 0;
			virtual void ReleaseVideoFrame(void* pFrame) = 0;
			// 创建解码器的状态，用于收到视频流时，判断是否先等待外部创建解码器
            			// -1: 未创建；0：正在创建； 1：创建成功； 2：创建失败

			#endif
			virtual void RecieveInputUserInfoEnd(int type) = 0;
#ifdef ANDROID
			virtual void onPopupSoftKeyboard() = 0;
#endif
			
	virtual void SetDecoderStatus(int iStatus) = 0;
		};
	}
}

#endif // BASE_INSTANCE_ABSTRACT_H

