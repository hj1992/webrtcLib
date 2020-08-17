package com.gwecom.webrtcmodule.WebRtcClient;

import android.util.Log;
import org.webrtc.OriginalAudioSource;

import java.nio.ByteBuffer;

import static com.gwecom.webrtcmodule.NativeClient.WebrtcModule.RecieveAudioStream;

public class OriginalAudioSourceImpl
        implements OriginalAudioSource
{

    private static final String TAG = OriginalAudioSourceImpl.class.getSimpleName();

    private  long lClientHandle=0;

  public   OriginalAudioSourceImpl(long lclientHandle)
    {
        lClientHandle=lclientHandle;
    }

  public int onAudioSource(ByteBuffer var1, int var2, int var3)
    {
        byte[] pData = new byte[var1.remaining()];
        var1.get(pData, 0, pData.length);

        RecieveAudioStream(lClientHandle,pData,pData.length);
        Log.d(TAG, "get audio data:" + pData.length);
return  0;
    }
}
