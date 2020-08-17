package com.gwecom.webrtcmodule;

import android.util.Log;

import org.webrtc.Logging;

public class WebrtcLogMessage
        implements org.webrtc.Loggable
{

    private static final String TAG = WebrtcLogMessage.class.getSimpleName();
    @Override
    public void onLogMessage(String var1, Logging.Severity var2, String var3)
    {
        Log.i(TAG, "onLogMessage:"+var2+var1+var3);
    }
}
