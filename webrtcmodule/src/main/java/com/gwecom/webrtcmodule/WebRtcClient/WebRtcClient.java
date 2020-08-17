package com.gwecom.webrtcmodule.WebRtcClient;

import android.content.Context;

import android.os.Build;
import android.support.annotation.Nullable;
import android.support.annotation.RequiresApi;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;
import org.webrtc.DataChannel;
import org.webrtc.IceCandidate;
import org.webrtc.MediaConstraints;
import org.webrtc.MediaStream;
import org.webrtc.PeerConnection;
import org.webrtc.PeerConnectionFactory;
import org.webrtc.RTCStatsCollectorCallback;
import org.webrtc.RTCStatsReport;
import org.webrtc.RtpReceiver;
import org.webrtc.SdpObserver;
import org.webrtc.SessionDescription;
import org.webrtc.VideoDecoderFactory;
import org.webrtc.VideoEncoderFactory;

import java.net.Inet6Address;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Enumeration;
import java.util.Iterator;
import java.util.List;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.gwecom.webrtcmodule.NativeClient.IWebRtcClientCallBack;
import com.gwecom.webrtcmodule.NativeClient.WebrtcModule;

import static com.gwecom.webrtcmodule.NativeClient.WebrtcModule.InitSignaling;
import static com.gwecom.webrtcmodule.NativeClient.WebrtcModule.SetClientCallBack;
import static com.gwecom.webrtcmodule.NativeClient.WebrtcModule.WebrtcMessage;
import static com.gwecom.webrtcmodule.NativeClient.WebrtcModule.RecieveRtt;

import org.webrtc.SoftwareVideoEncoderFactory;
import org.webrtc.audio.AudioDeviceModule;
import org.webrtc.audio.JavaAudioDeviceModule;
import java.util.Base64;

public class WebRtcClient
        implements IWebRtcClientCallBack {



    private static final String TAG = WebRtcClient.class.getSimpleName();
    private PeerConnectionFactory factory=null;
    private List<PeerConnection.IceServer> iceServers = new ArrayList<>();
    private MediaConstraints constraints = new MediaConstraints();
    private Peer janusPeer=null;
    private String janusSdp = "";
    private AudioDeviceModule adm=null;
    private Context context=null;
    private  long lClientHandle=0;
    private int nIceTransportsType=0;




    public WebRtcClient(long lclientHandle,Context context)
    {
        this.context=context;
        this.lClientHandle=lclientHandle;
        SetClientCallBack(lClientHandle,this);
        InitSignaling(lClientHandle);
    }


    //<--IWebRtcClientCallBack
  @Override
  public   void InitPeerConnection(String strUsername,String strCredential,String strTurnSrvUrl,int nIceTransportsType,boolean bFinish)
  {
      //add turn
      if (!strTurnSrvUrl.isEmpty() && !strUsername.isEmpty()&&!strCredential.isEmpty()) {
         PeerConnection.IceServer turnpeerIceServer = PeerConnection.IceServer.builder(strTurnSrvUrl)
                 .setUsername(strUsername)
                  .setPassword(strCredential)
                 .createIceServer();
         iceServers.add(turnpeerIceServer);
      }
      else if(!strTurnSrvUrl.isEmpty() && strUsername.isEmpty()&&strCredential.isEmpty())
      {
         PeerConnection.IceServer turnpeerIceServer = PeerConnection.IceServer.builder(strTurnSrvUrl)
                 .createIceServer();
         iceServers.add(turnpeerIceServer);
      }

      Log.d(TAG, "InitPeerConnection: " + strTurnSrvUrl +" bFinish:"+bFinish+"\n");
      if (bFinish)
      {
          constraints.mandatory.add(new MediaConstraints.KeyValuePair("OfferToReceiveAudio", "true"));
          constraints.mandatory.add(new MediaConstraints.KeyValuePair("OfferToReceiveVideo", "true"));
          this.nIceTransportsType=nIceTransportsType;

          InitPeerConnection();
         if(janusSdp.length()>0){
             SessionDescription sdp = new SessionDescription(
                     SessionDescription.Type.ANSWER,
                     janusSdp);
             janusPeer.pc.setRemoteDescription(janusPeer, sdp);
         }
      }

  }

    public   void InitPeerConnection()
    {
        if(factory==null) {
            PeerConnectionFactory.Options options = new PeerConnectionFactory.Options();
            // options.networkIgnoreMask = 0;
            // options.disableNetworkMonitor = true;
            //AudioOriginalFactory();
            final VideoEncoderFactory encoderFactory = new SoftwareVideoEncoderFactory();
          //  final VideoDecoderFactory decoderFactory = new SoftwareVideoDecoderFactory();
            adm = JavaAudioDeviceModule.builder(context)
                    .setUseStereoOutput(true)
                    .setUseStereoInput(true)
                    .setOutputSampleRate(48000)
                    .setSampleRate(48000)
                    .setInputSampleRate(48000)
                    .setUseHardwareAcousticEchoCanceler(false)
                    .setUseHardwareNoiseSuppressor(false)
                    .createAudioDeviceModule();

             adm.setMicrophoneMute(false);



            factory = PeerConnectionFactory.builder()
                    .setOptions(options)
                    .setAudioDeviceModule(adm)
                    .setVideoDecoderFactory((VideoDecoderFactory) new VideoDecoderFactoryImpl(lClientHandle))
                    .setVideoEncoderFactory(encoderFactory)
                    //.setAudioProcessingFactory()
                  //  .setVideoDecoderFactory(decoderFactory)

                    .createPeerConnectionFactory();

           factory.SetOriginalAudioSource(new OriginalAudioSourceImpl(lClientHandle));
        }


        Log.d(TAG, "InitPeerConnection(): "+ "\n");
       if(janusPeer==null)
          janusPeer = new Peer();
    }



    public void UnInitPeerConnection()
    {
        Log.d(TAG, "UnInitPeerConnection: "+ "\n");
        if(janusPeer!=null)
        {
            Log.d(TAG, "UnInitPeerConnection:  janusPeer.release"+ "\n");
            janusPeer.release();

            janusPeer = null;
        }
        if(factory!=null)
        {
            factory.dispose();
            factory=null;
        }
        if (adm!=null)
        {
            adm.release();
            adm=null;
        }
        com.gwecom.webrtcmodule.NativeClient.WebrtcModule.DestroyClient(lClientHandle);
    }

  @Override
  public   void  ConnectJanus(String strSdp)
  {
      //InitPeerConnection();
      Log.d(TAG, "ConnectJanus OFFER SDP: " + strSdp+"\n");

    janusSdp =strSdp;// preferCodec(strSdp, "H264", false);;


     // janusSdp=janusSdp.replaceAll("H264","H265");
      if (!janusSdp.equals(strSdp))
          Log.d(TAG, "fix offer sdp: " + janusSdp+"\n");
 if(janusPeer!=null
         && janusPeer.pc!=null
         && !janusSdp.isEmpty()) {
     SessionDescription sdp = new SessionDescription(
             SessionDescription.Type.OFFER,
             janusSdp);
     janusPeer.pc.setRemoteDescription(janusPeer, sdp);
 }

  }

    @Override
  public void WebrtcSendData(String strData)
  {
      //Peer peer = getPeer("01");
      if(janusPeer!=null)
      janusPeer.sendDataChannelMessage(strData);
  }

    @RequiresApi(api = Build.VERSION_CODES.O)
    @Override
  public void WebrtcSendData(byte[] pdata,int ndatalen,boolean bBinary)
  {
      if(janusPeer!=null)
          janusPeer.sendDataChannelMessage(pdata,ndatalen,bBinary);
  }

  @Override
    public void  RttRequest()
    {
        if(janusPeer!=null
                && janusPeer.pc!=null)
        janusPeer.RttRequest();
    }
  //-->

    private static String joinString(
            Iterable<? extends CharSequence> s, String delimiter, boolean delimiterAtEnd) {
        Iterator<? extends CharSequence> iter = s.iterator();
        if (!iter.hasNext()) {
            return "";
        }
        StringBuilder buffer = new StringBuilder(iter.next());
        while (iter.hasNext()) {
            buffer.append(delimiter).append(iter.next());
        }
        if (delimiterAtEnd) {
            buffer.append(delimiter);
        }
        return buffer.toString();
    }

    /** Returns the line number containing "m=audio|video", or -1 if no such line exists. */
    private static int findMediaDescriptionLine(boolean isAudio, String[] sdpLines) {
        final String mediaDescription = isAudio ? "m=audio " : "m=video ";
        for (int i = 0; i < sdpLines.length; ++i) {
            if (sdpLines[i].startsWith(mediaDescription)) {
                return i;
            }
        }
        return -1;
    }

    private static @Nullable
    String movePayloadTypesToFront(
            List<String> preferredPayloadTypes, String mLine) {
        // The format of the media description line should be: m=<media> <port> <proto> <fmt> ...
        final List<String> origLineParts = Arrays.asList(mLine.split(" "));
        if (origLineParts.size() <= 3) {
            Log.e(TAG, "Wrong SDP media description format: " + mLine);
            return null;
        }
        final List<String> header = origLineParts.subList(0, 3);
        final List<String> unpreferredPayloadTypes =
                new ArrayList<>(origLineParts.subList(3, origLineParts.size()));
        unpreferredPayloadTypes.removeAll(preferredPayloadTypes);
        // Reconstruct the line with |preferredPayloadTypes| moved to the beginning of the payload
        // types.
        final List<String> newLineParts = new ArrayList<>();
        newLineParts.addAll(header);
        newLineParts.addAll(preferredPayloadTypes);
        newLineParts.addAll(unpreferredPayloadTypes);
        return joinString(newLineParts, " ", false /* delimiterAtEnd */);
    }


    private static String preferCodec(String sdpDescription, String codec, boolean isAudio) {
        final String[] lines = sdpDescription.split("\r\n");
        final int mLineIndex = findMediaDescriptionLine(isAudio, lines);
        if (mLineIndex == -1) {
            Log.w(TAG, "No mediaDescription line, so can't prefer " + codec);
            return sdpDescription;
        }
        // A list with all the payload types with name |codec|. The payload types are integers in the
        // range 96-127, but they are stored as strings here.
        final List<String> codecPayloadTypes = new ArrayList<>();
        // a=rtpmap:<payload type> <encoding name>/<clock rate> [/<encoding parameters>]
        final Pattern codecPattern = Pattern.compile("^a=rtpmap:(\\d+) " + codec + "(/\\d+)+[\r]?$");
        for (String line : lines) {
            Matcher codecMatcher = codecPattern.matcher(line);
            if (codecMatcher.matches()) {
                codecPayloadTypes.add(codecMatcher.group(1));
            }
        }
        if (codecPayloadTypes.isEmpty()) {
            Log.w(TAG, "No payload types with name " + codec);
            return sdpDescription;
        }

        final String newMLine = movePayloadTypesToFront(codecPayloadTypes, lines[mLineIndex]);
        if (newMLine == null) {
            return sdpDescription;
        }
        Log.d(TAG, "Change media description from: " + lines[mLineIndex] + " to " + newMLine);
        lines[mLineIndex] = newMLine;
        return joinString(Arrays.asList(lines), "\r\n", true /* delimiterAtEnd */);
    }


  public class Peer
          implements SdpObserver,
          PeerConnection.Observer,
          DataChannel.Observer,
          RTCStatsCollectorCallback {

      PeerConnection pc=null;
      //String id;
      DataChannel dc=null;
      boolean bGetRTTStats=false;


      public Peer() {

          PeerConnection.RTCConfiguration rtccfg=new PeerConnection.RTCConfiguration(iceServers);
          rtccfg.iceTransportsType=PeerConnection.IceTransportsType.RELAY;
          switch (nIceTransportsType)
          {
              case 2:
                  rtccfg.iceTransportsType=PeerConnection.IceTransportsType.NOHOST;
                  break;
              case 3:
                  rtccfg.iceTransportsType=PeerConnection.IceTransportsType.RELAY;
                  break;
                  default:
                      break;
          }

          Log.d(TAG, "nIceTransportsType:"+nIceTransportsType+"   rtccfg.iceTransportsType:"+rtccfg.iceTransportsType+ "\n");

          rtccfg.bundlePolicy=PeerConnection.BundlePolicy.MAXCOMPAT;
         rtccfg.tcpCandidatePolicy=PeerConnection.TcpCandidatePolicy.DISABLED;
         // rtccfg.enableDtlsSrtp=true;
         // rtccfg.continualGatheringPolicy= PeerConnection.ContinualGatheringPolicy.GATHER_CONTINUALLY;
       //  rtccfg.suspendBelowMinBitrate=true;
          //rtccfg.rtcpMuxPolicy=PeerConnection.RtcpMuxPolicy.NEGOTIATE;
          //rtccfg.turnPortPrunePolicy = PeerConnection.PortPrunePolicy.KEEP_FIRST_READY;
         // rtccfg.candidateNetworkPolicy=PeerConnection.CandidateNetworkPolicy.LOW_COST;
         //rtccfg.iceConnectionReceivingTimeout = 3000;
        // rtccfg.iceUnwritableTimeMs=3000;
         //rtccfg.iceUnwritableMinChecks=5;
         //rtccfg.suspendBelowMinBitrate=true;
      // rtccfg.iceBackupCandidatePairPingInterval =6000;
         // rtccfg.networkPreference = PeerConnection.AdapterType.WIFI;
          rtccfg.disableIpv6=true;
          rtccfg.disableIPv6OnWifi=true;

          this.pc = factory.createPeerConnection(
                  rtccfg,
                  this);
         // this.pc.createOffer(this,constraints);
      }

      public void CreateDataChannel()
      {
                      /*
            DataChannel.Init 可配参数说明：
            ordered：是否保证顺序传输；
            maxRetransmitTimeMs：重传允许的最长时间；
            maxRetransmits：重传允许的最大次数；
             */
            if(pc==null)
              return;
          DataChannel.Init init = new DataChannel.Init();
          init.ordered = false;
          dc = pc.createDataChannel("JanusDataChannel", init);

          if (dc!=null)
            dc.registerObserver(this);
      }

      public void CloseDataChannel()
      {
          if(pc==null)
              return;

          if (dc!=null)
          {
              dc.unregisterObserver();
              dc.close();
              dc.dispose();

              dc=null;
          }
      }

      public void sendDataChannelMessage(String strData)
      {
          if(dc!=null
                  && dc.state()==DataChannel.State.OPEN)
          {
              boolean isendflags = (strData.lastIndexOf(0)=='\0');
              if(!isendflags)
              {
                  strData+='\0';
              }

              DataChannel.Buffer buffer = new DataChannel.Buffer(
                      ByteBuffer.wrap( strData.getBytes()),
                      false);
              dc.send(buffer);
          }


      }

      @RequiresApi(api = Build.VERSION_CODES.O)
      public void sendDataChannelMessage(byte[] pdata, int ndatalen, boolean bBinary)
      {
          if (dc!=null
                  && dc.state()==DataChannel.State.OPEN)
          {
          //    Log.d(TAG, "sendDataChannelMessage:"+" ndatalen:"+ndatalen+" bBinary:"+bBinary+"\n");
              DataChannel.Buffer buffer = new DataChannel.Buffer(
                      ByteBuffer.wrap(pdata,0,ndatalen),
                      bBinary);
          //    String binaryString = Base64.getEncoder().encodeToString(pdata);
          boolean bRes= dc.send(buffer);
            //  Log.d(TAG, "sendDataChannelMessage:"+" ndatalen:"+ndatalen+" bBinary:"+bBinary+" bres:"+bRes+"\n"+"binaryString: "+binaryString);
          }
          else
          {
              Log.d(TAG, "sendDataChannelMessage:Failed");
          }
      }

      public void release() {

          Log.d(TAG, "release check bGetRTTStats"+ "\n");
          int nidx=0;
          while (bGetRTTStats)
          {
              try {
                  Thread.sleep(1);
                  Log.v(TAG, "release  sleep ");
                  if (nidx++>100)
                  {
                      break;
                  }
              }
              catch (Exception ex)
              {
                  ex.printStackTrace();
              }
         }

          Log.d(TAG, "release"+ "\n");
          if (dc!=null)
          {
              Log.d(TAG, "release dc"+ "\n");
              dc.unregisterObserver();
              dc.close();
              dc.dispose();
              dc=null;
          }

if (pc!=null)
{
    Log.d(TAG, "release pc"+ "\n");
    pc.close();
    pc.dispose();
    pc=null;
}

      }

      //SdpObserver-------------------------------------------------------------------------------

      public  String getHostIP() {

          String hostIp = null;
          try {
              Enumeration nis = NetworkInterface.getNetworkInterfaces();
              InetAddress ia = null;
              while (nis.hasMoreElements()) {
                  NetworkInterface ni = (NetworkInterface) nis.nextElement();
                  Enumeration<InetAddress> ias = ni.getInetAddresses();
                  while (ias.hasMoreElements()) {
                      ia = ias.nextElement();
                      if (ia instanceof Inet6Address) {
                          continue;// skip ipv6
                      }
                      String ip = ia.getHostAddress();
                      if (!"127.0.0.1".equals(ip)) {
                          hostIp = ia.getHostAddress();
                          break;
                      }
                  }
              }
          } catch (SocketException e) {
              Log.i("yao", "SocketException");
              e.printStackTrace();
          }
          return hostIp;

      }

      @Override
      public  void onCreateSuccess(SessionDescription sdp) {

          if (pc==null)
          {
              return;
          }
          if (sdp.type==SessionDescription.Type.ANSWER)
          {
              Log.d(TAG, "onCreateSuccess: " + sdp);
              pc.setLocalDescription(Peer.this, sdp);
              Log.d(TAG, "getLocalDescription: "+sdp.description);

              WebrtcModule.SendAnswer( lClientHandle,sdp.description);
//              String tmpsdp = sdp.description.replace("a=mid:video\n" +
//                      "    a=extmap:5 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\n" +
//                      "    a=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay\n" +
//                      "    a=recvonly\n" +
//                      "    a=rtcp-mux","a=mid:video\n" +
//                      "    a=extmap:5 http://www.ietf.org/id/draft-holmer-rmcat-transport-wide-cc-extensions-01\n" +
//                      "    a=extmap:6 http://www.webrtc.org/experiments/rtp-hdrext/playout-delay\n" +
//                      "    a=recvonly\n" +
//                      "    a=rtcp-mux\n" +
//                      "a=rtpmap:126 H264/90000\n" +
//                      "a=rtcp-fb:126 goog-remb\n" +
//                      "a=rtcp-fb:126 transport-cc\n" +
//                      "a=rtcp-fb:126 nack\n" +
//                      "a=fmtp:126 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");
//              tmpsdp = tmpsdp.replace("m=video 9 UDP/TLS/RTP/SAVPF 0", "m=video 9 UDP/TLS/RTP/SAVPF 126");
//              SessionDescription sdp1 = new SessionDescription(
//                      sdp.type,
//                      tmpsdp);
//              pc.setLocalDescription(Peer.this, sdp1);
//              WebrtcModule.SendAnswer( sdp1.description);
//              Log.d(TAG, "setLocalDescription: "+sdp1.description);

          }
          else
          {
              //pc.setLocalDescription(Peer.this, sdp);
              String tmpsdp = sdp.description.replace("a=rtcp-mux","a=rtcp-mux\n" +
                      "a=rtpmap:126 H264/90000\n" +
                      "a=rtcp-fb:126 goog-remb\n" +
                      "a=rtcp-fb:126 transport-cc\n" +
                      "a=rtcp-fb:126 nack\n" +
                      "a=fmtp:126 level-asymmetry-allowed=1;packetization-mode=1;profile-level-id=42e01f");
              tmpsdp = tmpsdp.replace("m=video 9 UDP/TLS/RTP/SAVPF 0", "m=video 9 UDP/TLS/RTP/SAVPF 126");
              SessionDescription sdp1 = new SessionDescription(
                      sdp.type,
                      tmpsdp);
              pc.setLocalDescription(Peer.this, sdp1);

              Log.d(TAG, "setLocalDescription: "+sdp1.description);
          }
      }



      @Override
      public void onSetSuccess() {
          if (pc==null)
          {
              return;
          }
          Log.v(TAG, "onSetSuccess   " + pc.signalingState().toString());
          if (pc.signalingState().toString().equals("HAVE_REMOTE_OFFER")){
              janusPeer.pc.createAnswer(janusPeer,constraints);
          }

      }

      @Override
      public void onCreateFailure(String s) {
          if (pc==null)
          {
              return;
          }
          Log.d(TAG, "onCreateFailure: " + s);
          WebrtcModule.OnWebrtcError(lClientHandle,-65);
      }

      @Override
      public void onSetFailure(String s) {
          if (pc==null)
          {
              return;
          }
          Log.d(TAG, "onSetFailure: " + s);
          WebrtcModule.OnWebrtcError(lClientHandle,-66);
      }

      //DataChannel.Observer----------------------------------------------------------------------

      @Override
      public void onBufferedAmountChange(long l) {

      }

      @Override
      public void onStateChange() {

          Log.d(TAG, "onDataChannel onStateChange:" + dc.state()
          );
      }

      @Override
      public void onMessage(DataChannel.Buffer buffer) {

          ByteBuffer data = buffer.data;

          byte[] bytes = new byte[data.capacity()];
          data.get(bytes);
          Log.d(TAG, "onDataChannel onMessage : " + bytes.length+" buffer.binary:"+buffer.binary);
         WebrtcMessage(lClientHandle,bytes,bytes.length,buffer.binary);


      }

      @Override
      public void onStatsDelivered(RTCStatsReport var1)
      {

          for (String key : var1.getStatsMap().keySet()) {
             if (key.indexOf("RTCIceCandidatePair")==0
                     &&var1.getStatsMap().get(key).getMembers().get("state").toString().contains("succeeded"))
              {
                  String strCurrentRoundTripTime=var1.getStatsMap().get(key).getMembers().get("currentRoundTripTime").toString();
                  if (!strCurrentRoundTripTime.isEmpty()) {
                      float fRoundTripTime = Float.valueOf(strCurrentRoundTripTime);
                      fRoundTripTime*=1000;
                      long lRoundTripTime=(long) (fRoundTripTime);
                      Log.d(TAG, "RttRequest onStatsDelivered RTCStatsReport:" + key + "     " +  strCurrentRoundTripTime);
                      RecieveRtt(lClientHandle,lRoundTripTime);
                  }
              }

          }
          bGetRTTStats=false;
      }

      public void RttRequest()
      {
          if (bGetRTTStats)
          {
              return;
          }
          if (pc!=null)
          {
              Log.d(TAG, "Call RttRequest");
              bGetRTTStats=true;
           pc.getStats(this);
          }
      }

      //PeerConnection.Observer-------------------------------------------------------------------

      @Override
      public void onSignalingChange(PeerConnection.SignalingState signalingState) {

      }

      @Override
      public void onIceConnectionChange(PeerConnection.IceConnectionState iceConnectionState) {
          Log.d(TAG, "onIceConnectionChange : " + iceConnectionState.name());
          if (iceConnectionState == PeerConnection.IceConnectionState.CONNECTED)
          {
              WebrtcModule.StartKeepAlive(lClientHandle);
              //Peer peer = getPeer("JanusDataChannel");
              janusPeer.CreateDataChannel();

          }
          else if (iceConnectionState == PeerConnection.IceConnectionState.DISCONNECTED
                  ||iceConnectionState == PeerConnection.IceConnectionState.CLOSED
                  || iceConnectionState == PeerConnection.IceConnectionState.FAILED)
          {
              WebrtcModule.StopKeepAlive(lClientHandle);
              janusPeer.CloseDataChannel();
          }
      }

      @Override
      public void onIceConnectionReceivingChange(boolean b) {

      }

      @Override
      public void onIceGatheringChange(PeerConnection.IceGatheringState iceGatheringState) {
          Log.d(TAG, "onIceGatheringChange : " + iceGatheringState);
          if (iceGatheringState == PeerConnection.IceGatheringState.COMPLETE)
          {
              try {
              JSONObject payload = new JSONObject();
              payload.put("completed", true);

              WebrtcModule.SendTrickle(lClientHandle,payload.toString());
              } catch (JSONException e) {
                  e.printStackTrace();
              }
          }

      }

      @Override
      public void onIceCandidate(IceCandidate candidate) {

          //Peer peer = getPeer("01");
          if (janusPeer.pc.getRemoteDescription() != null) {
             /* IceCandidate candidateitem = new IceCandidate(
                      candidate.sdpMid,
                      candidate.sdpMLineIndex,
                      candidate.sdp
              );*/
              try {
                  JSONObject payload = new JSONObject();
                  payload.put("candidate", candidate.sdp);
                  payload.put("sdpMid", candidate.sdpMid);
                  payload.put("sdpMLineIndex", candidate.sdpMLineIndex);

                  Log.d(TAG, "payload : " + payload.toString());

                  WebrtcModule.SendTrickle(lClientHandle,payload.toString());

              } catch (JSONException e) {
                  e.printStackTrace();
              }

             pc.addIceCandidate(candidate);

          }
      }

      @Override
      public void onIceCandidatesRemoved(IceCandidate[] iceCandidates) {

      }

      @Override
      public void onAddStream(MediaStream mediaStream) {
          Log.d(TAG, "onAddStream label:" + mediaStream.toString()+"\n");
      }

      @Override
      public void onRemoveStream(MediaStream mediaStream) {

      }

      @Override
      public void onDataChannel(DataChannel dataChannel) {
          Log.d(TAG, "onDataChannel label:" + dataChannel.label()+"\n");
          dataChannel.registerObserver(this);
      }

      @Override
      public void onRenegotiationNeeded() {

      }

      @Override
      public void onAddTrack(RtpReceiver rtpReceiver, MediaStream[] mediaStreams) {
          if (mediaStreams.length>0)
          {
              Log.d(TAG, "onAddTrack label:" + mediaStreams[0].toString()+"\n");
          }
          Log.d(TAG, "onAddTrack label:" + mediaStreams.toString()+"\n");

      }
  }
}


