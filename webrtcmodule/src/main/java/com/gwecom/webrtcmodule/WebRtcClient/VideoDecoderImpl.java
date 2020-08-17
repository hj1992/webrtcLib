package com.gwecom.webrtcmodule.WebRtcClient;

import android.util.Log;

import org.webrtc.EncodedImage;
import org.webrtc.VideoCodecStatus;
import org.webrtc.VideoDecoder;

import android.os.Environment;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

import static com.gwecom.webrtcmodule.NativeClient.WebrtcModule.RecieveVideoStream;

public class VideoDecoderImpl implements VideoDecoder
{
    private static final String TAG = VideoDecoderImpl.class.getSimpleName();

    private  long lClientHandle=0;

    public void setClientHandle(long lclientHandle)
    {
        this.lClientHandle=lclientHandle;
    }

    public long createNativeVideoDecoder()
    {
        return 0;
    }

    int width, height;
  public   VideoCodecStatus initDecode(VideoDecoder.Settings var1, VideoDecoder.Callback var2)
    {
        Log.i(TAG, "initDecode: \n"+var1.numberOfCores+" _ "+var1.width+"_ "+var1.height);
        return VideoCodecStatus.OK;
    }

   public VideoCodecStatus release()
   {
       return VideoCodecStatus.OK;
   }

    public VideoCodecStatus decode(EncodedImage pFrame, VideoDecoder.DecodeInfo info)
    {
        byte[] pData = new byte[pFrame.buffer.remaining()];
        pFrame.buffer.get(pData, 0, pData.length);

       RecieveVideoStream(lClientHandle,pData,pData.length);
        Log.d(TAG, "get video data:" + pData.length);


//        try {
//            File parent_path = Environment.getExternalStorageDirectory();
//            File file = new File(parent_path, "videoData.H265");
//            file.createNewFile();
//            Log.d("文件路径", file.getAbsolutePath());
//            FileOutputStream fos = new FileOutputStream(file,true);
//
//            fos.write(pData, 0, pData.length);
//            fos.flush();
//            fos.close();
//        }catch (IOException ex)
//        {
//
//        }

        return VideoCodecStatus.OK;
    }

   public boolean getPrefersLateDecoding()
   {
       return true;
   }

   public String getImplementationName()
   {
       return "CusVideoDecoder";
   }

}
