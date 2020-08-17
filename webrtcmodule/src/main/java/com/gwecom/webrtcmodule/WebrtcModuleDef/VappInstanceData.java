package com.gwecom.webrtcmodule.WebrtcModuleDef;

import com.alibaba.fastjson.JSON;
import com.alibaba.fastjson.TypeReference;

public class  VappInstanceData
{
    public boolean bUseSSL;
    public int frameWidth;
    public int frameHeight;
    public int useWebrtc;
    public int useTurnServer;
    public String turnServerIP;
    public int turnServerPort;
    public String instanceKey;		//通过后台获取到的实例ID，如果为空，那么表示是直连版本
    public String serverDomainUrl;	// tcp 推流服务器地址
    public int serverDomainPort;	// tcp 推流服务器端口
    public String controlImgUrl;
    public int screenDirect;		// 0:横屏, 1:竖屏
    public int connectType;			// 连接方式（0.应用程序1.桌面）
    public int accountType;		// 游戏账号：1steam账号2暴雪
    //virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
    //webrtc 连接信息
    public String janusIP;    // janus http address
    public int janusPort;  // janus http port
    public String iceServerInfo;
    public int videoCodec;

    public String ToJsonString()
    {
        return  JSON.toJSON(this).toString();
    }

    public static VappInstanceData Parse(String strjson)
    {
        return JSON.parseObject(strjson,new TypeReference<VappInstanceData>(){});
    }
}
