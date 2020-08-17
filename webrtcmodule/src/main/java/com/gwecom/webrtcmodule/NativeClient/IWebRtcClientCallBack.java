package com.gwecom.webrtcmodule.NativeClient;

public interface IWebRtcClientCallBack
{
   void InitPeerConnection(String strUsername,String strCredential,String strTurnSrvUrl,int nIceTransportsType,boolean bFinish);
   void  InitPeerConnection();
   void  ConnectJanus(String strSdp);
   void WebrtcSendData(String strData);
   void WebrtcSendData(byte[] pdata,int ndatalen,boolean bBinary);
   void  RttRequest();
}
