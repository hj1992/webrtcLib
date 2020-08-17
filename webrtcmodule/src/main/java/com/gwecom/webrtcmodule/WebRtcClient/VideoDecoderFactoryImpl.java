package com.gwecom.webrtcmodule.WebRtcClient;

import android.support.annotation.Nullable;
import android.util.Log;

import org.webrtc.VideoCodecInfo;
import org.webrtc.VideoDecoder;
import org.webrtc.VideoDecoderFactory;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class VideoDecoderFactoryImpl
        implements VideoDecoderFactory
{
    private static final String TAG = VideoDecoderFactoryImpl.class.getSimpleName();
    private VideoDecoderImpl CusvideoDecoder=null;
    private  long lClientHandle=0;
    //private VideoCodecInfo[] info  = new VideoCodecInfo[1];

    public VideoDecoderFactoryImpl(long lclientHandle)
    {
        Log.i(TAG,"VideoDecoderFactoryImpl created");
        setClientHandle(lclientHandle);
    }

    public void setClientHandle(long lclientHandle)
    {
        this.lClientHandle=lclientHandle;
        if (CusvideoDecoder!=null)
        {
            CusvideoDecoder.setClientHandle(lclientHandle);
        }
    }


    @Nullable
    public VideoDecoder createDecoder(VideoCodecInfo info) {
        Log.i(TAG,"createDecoder:"+info.name);
        if (info.name.equals("H264") )
        {
            Log.i(TAG,"createDecoder:"+info.name);
            CusvideoDecoder=new VideoDecoderImpl();
            if (CusvideoDecoder!=null)
            {
                CusvideoDecoder.setClientHandle(lClientHandle);
            }
            //this.info[0]=info;
            return CusvideoDecoder;
        }
        else if (info.name.equals("H265") )
        {
            Log.i(TAG,"createDecoder:"+info.name);
            CusvideoDecoder=new VideoDecoderImpl();
            if (CusvideoDecoder!=null)
            {
                CusvideoDecoder.setClientHandle(lClientHandle);
            }
            //this.info[0]=info;
            return CusvideoDecoder;
        }
        return null;
    }

    public VideoCodecInfo[] getSupportedCodecs() {
        return supportedCodecs();
    }

    static VideoCodecInfo[] supportedCodecs() {
        Log.i(TAG,"called supportedCodecs");
        List<VideoCodecInfo> codecs = new ArrayList();
        codecs.add(new VideoCodecInfo("H264", new HashMap()));
        codecs.add(new VideoCodecInfo("H265", new HashMap()));


        return (VideoCodecInfo[])codecs.toArray(new VideoCodecInfo[codecs.size()]);
    }
}


