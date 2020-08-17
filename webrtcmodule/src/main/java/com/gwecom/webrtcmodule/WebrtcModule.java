package com.gwecom.webrtcmodule;

import android.app.Activity;
import android.content.Context;
import android.os.Build;
import android.support.annotation.RequiresApi;
import android.support.v4.app.Fragment;
import android.util.Log;

import com.gwecom.webrtcmodule.PermissionUtils.PermissionHelper;
import com.gwecom.webrtcmodule.PermissionUtils.PermissionUtil;
import com.gwecom.webrtcmodule.PermissionUtils.bean.PermissionModel;
import com.gwecom.webrtcmodule.PermissionUtils.callback.PermissionCallback;
import com.gwecom.webrtcmodule.WebRtcClient.WebRtcClient;
import com.gwecom.webrtcmodule.WebrtcModuleDef.VappInstanceData;

import org.webrtc.Logging;
import org.webrtc.PeerConnectionFactory;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import static android.content.res.AssetManager.ACCESS_STREAMING;

public class WebrtcModule
        implements PermissionCallback
{
    private static final String TAG = WebrtcModule.class.getSimpleName();
    private WebRtcClient Client=null;
    private Context context=null;
    private String m_strCacertFilePath;
    private static final String DISABLE_WEBRTC_AGC_FIELDTRIAL =
            "WebRTC-Audio-MinimizeResamplingOnMobile/Enabled/";
    private static final String VIDEO_FLEXFEC_FIELDTRIAL =
            "WebRTC-FlexFEC-03-Advertised/Enabled/WebRTC-FlexFEC-03/Enabled/";
    private boolean bInitializationOptions=false;
    private   boolean bCheckPermissions=false;

    public WebrtcModule(Context context)  {

        bInitializationOptions=false;
        if (this.context==null)
        {
            this.context=context;
        }
    }


    public boolean CheckRecordAudioPermissions(Context context,boolean brequest)
    {
        Context tmpContext;
        Object container=(Object) context;

        if (container instanceof Activity)
        {
            tmpContext=(Activity) context;
        } else if (container instanceof Fragment)
    {
        tmpContext=((Fragment) container).getContext();
    }
        else
        {
            tmpContext=context;
        }

        boolean bCheckRecordAudioPermissions=true;
        String[] strPermis={PermissionModel.RECORD_AUDIO};
        PermissionHelper.getInstance().with((Activity)context);
        PermissionHelper.getInstance().setPermissions(strPermis);
        PermissionHelper.getInstance().setPermissonCallback(this);
        if (brequest)
        {
            if(PermissionHelper.getInstance().requestPermissions(true))
            {
                bCheckRecordAudioPermissions=true;
            }
        }
        else
        {
            bCheckRecordAudioPermissions=PermissionUtil.checkSelfPermissions(context,strPermis);
        }

        return bCheckRecordAudioPermissions;
    }

    public boolean CheckPermissions(Context context,boolean brequest)
    {
        bCheckPermissions=false;
        String[] strPermis={PermissionModel.RECORD_AUDIO, PermissionModel.READ_EXTERNAL_STORAGE,PermissionModel.WRITE_EXTERNAL_STORAGE};
        PermissionHelper.getInstance().with((Activity)context);
        PermissionHelper.getInstance().setPermissions(strPermis);
        PermissionHelper.getInstance().setPermissonCallback(this);
        if (brequest)
        {
            if(PermissionHelper.getInstance().requestPermissions(true))
            {
                bCheckPermissions=true;
            }
        }
        else
        {
            bCheckPermissions=PermissionUtil.checkSelfPermissions(context,strPermis);
        }

        return bCheckPermissions;
    }


    public boolean InitWebrtcModule(String strappInstanceData, long baseInstance)
    {
        boolean bRes=false;
        Log.i(TAG, "InitWebrtcModule: json:"+strappInstanceData);

        if (!bCheckPermissions)
        {
            return  false;
        }
        if(this.context==null)
        {
            return  false;
        }
        if (!bInitializationOptions) {
            String fieldTrials = DISABLE_WEBRTC_AGC_FIELDTRIAL;
            PeerConnectionFactory.initialize(
                    PeerConnectionFactory.InitializationOptions.builder(context)
                            .setFieldTrials(fieldTrials)
                            .setEnableInternalTracer(true)
                            .setInjectableLogger(new WebrtcLogMessage(), Logging.Severity.LS_VERBOSE)
                            .createInitializationOptions());

        m_strCacertFilePath="";
        try {
            m_strCacertFilePath = CopyDataToSD(context, "webrtcmodule_cacert.dat");
        }
        catch (IOException ex)
        {
            Log.i(TAG, "strCacertFilePath: IOException:"+ex.getMessage());
        }
        Log.i(TAG, "strCacertFilePath:"+m_strCacertFilePath);
            bInitializationOptions=true;
        }

      long lclientHandle=com.gwecom.webrtcmodule.NativeClient.WebrtcModule.CreateClient(strappInstanceData,baseInstance,m_strCacertFilePath);
        if(Client==null
                && this.context!=null
                && lclientHandle!=0) {
            Client = new WebRtcClient(lclientHandle,this.context);
            bRes=true;
        }
        else
        {
            if (lclientHandle!=0)
            {
                com.gwecom.webrtcmodule.NativeClient.WebrtcModule.DestroyClient(lclientHandle);
            }
        }
        return  bRes;
    }

    public boolean InitWebrtcModule(VappInstanceData appInstanceData, long baseInstance)
    {
     //   Log.i(TAG, "InitWebrtcModule: json:"+appInstanceData.ToJsonString());
     //   com.gwecom.webrtcmodule.NativeClient.WebrtcModule.CreateClient( appInstanceData.ToJsonString(),baseInstance,m_strCacertFilePath);
        return  false;
    }

    public void WebrtcSendData(String strData)
    {
        if(Client!=null)
           Client.WebrtcSendData(strData);
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    public void WebrtcSendData(byte[] pdata, int ndatalen, boolean bBinary)
    {
        if(Client!=null)
            Client.WebrtcSendData(pdata,ndatalen,bBinary);
    }

    public void RttRequest()
    {
        if(Client!=null)
            Client.RttRequest();
    }

   public boolean Uninitialize()
        {
            if (Client!=null)
            {
                Client.UnInitPeerConnection();
                Client=null;
            }
         return  true;
    }

    private String CopyDataToSD(Context context, String strOutFileName) throws IOException
    {
        String strOutFilePath=context.getApplicationContext().getFilesDir().getAbsolutePath();
        strOutFilePath+="/Cache";

        Log.i(TAG, "strCacertFilePath_0:"+strOutFilePath);

        String strCacertFilePath="";
        File OutFilePath = new File(strOutFilePath);
        if (!OutFilePath.exists())
            OutFilePath.mkdir();

        if(strOutFilePath.lastIndexOf("/")!=0)
        {
            strOutFilePath+="/";
        }

        InputStream cacertInput;
        OutputStream cacertOutput = new FileOutputStream(strOutFilePath+strOutFileName);
        Log.i(TAG, "strCacertFilePath_1:");
        cacertInput = context.getAssets().open(strOutFileName,ACCESS_STREAMING);
        Log.i(TAG, "strCacertFilePath_2:");
        byte[] buffer = new byte[1024];
        int length = cacertInput.read(buffer);
        while(length > 0)
        {
            cacertOutput.write(buffer, 0, length);
            length = cacertInput.read(buffer);

            strCacertFilePath=strOutFilePath+strOutFileName;
            Log.i(TAG, "strCacertFilePath_3:");
        }
        Log.i(TAG, "strCacertFilePath_4:");
        cacertOutput.flush();
        cacertInput.close();
        cacertOutput.close();

        return strCacertFilePath;
    }


    public void onPermissionGranted()
    {
        Log.i(TAG, "InitWebrtcModule: onPermissionGranted");
    }

   public void onPermissionReject()
    {
        Log.i(TAG, "InitWebrtcModule: onPermissionReject");
    }

}
