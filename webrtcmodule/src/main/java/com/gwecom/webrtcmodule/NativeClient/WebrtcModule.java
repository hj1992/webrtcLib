package com.gwecom.webrtcmodule.NativeClient;


import com.gwecom.webrtcmodule.NativeClient.IWebRtcClientCallBack;
import com.gwecom.webrtcmodule.WebrtcModuleDef.VappInstanceData;

public class WebrtcModule {
    static {
        try {

            System.loadLibrary("jingle_peerconnection_so");
            System.loadLibrary("webrtcmodule-lib");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    public static native long CreateClient(String jsonappInstanceData, long baseInstance,String strCacertFilePath);
    public static native void InitSignaling(long lHandle);
    public static native void DestroyClient(long lHandle);
    public static native void SetClientCallBack(long lHandle,IWebRtcClientCallBack clientCallBack);
    public static native void SendAnswer(long lHandle,String sdp);
    public static native void SendTrickle(long lHandle,String candidateInfo);
    public static native  void StartKeepAlive(long lHandle);
    public static native  void StopKeepAlive(long lHandle);
    public static native void RecieveVideoStream(long lHandle,byte[] pData,int nDataLen);
    public static native void RecieveAudioStream(long lHandle,byte[] pData,int nDataLen);
    public static native void WebrtcMessage(long lHandle,byte[] pData,int nDataLen,boolean binary);
    public static native  void RecieveRtt(long lHandle,long lrtttime);
    public static native  void OnWebrtcError(long lHandle,int nerror);

}
