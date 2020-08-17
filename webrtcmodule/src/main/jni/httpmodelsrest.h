#ifndef HTTPMODELS_REST_H
#define HTTPMODELS_REST_H 
#if UseCppRest
#include <string>
#include <exception>
#include <cpprest/json.h>
#include <cpprest/details/basic_types.h>

namespace gwecom {
	namespace app {

#define get_json_string(object, key) \
    (object.at(key).is_string() ? object.at(key).as_string() : _XPLATSTR(""))

#define get_json_integer(object, key) \
    (object.at(key).is_integer() ? object.at(key).as_integer() : 0)

#define get_json_integer64(object, key) \
    (object.at(key).is_number() ? object.at(key).as_number().to_int64() : 0)

		using namespace std;

		class JsonDataBase {
		public:
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) = 0;
		};

		class HttpResponseBase : public JsonDataBase {
		public:
			int                 tokenStatus;
			int                 code;
			int                 apiCode;
			utility::string_t   message;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
		};

		class HttpResponseDetailBase : public JsonDataBase {
		public:
			int                 code;
			int                 apiCode;
			utility::string_t   message;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
		};

		class VappData : public JsonDataBase {
		public:
			utility::string_t name;
			utility::string_t iconSrc;
			utility::string_t appKey;
			utility::string_t instanceKey;
			utility::string_t runningTimes;

			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
		};
		class VappInstanceData : public JsonDataBase {
		public:
			int serverPort; // 连接服务器端口
			int turnPort;   // 转发服务器端口
			int stunPort;   // 穿透服务器端口
			int serverDomainPort;	// tcp 推流服务器端口
			int frameWidth;
			int frameHeight;
			utility::string_t serverAddress;    // 连接服务器地址
			utility::string_t turnAddress;      // 转发服务器地址
			utility::string_t stunAddress;      // 穿透服务器地址
			utility::string_t instanceKey;
			utility::string_t serverDomainUrl;	// tcp 推流服务器地址		
			utility::string_t controlImgUrl;
			int screenDirect;		// 0:横屏, 1:竖屏


			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
		};
		class VappResponse : public HttpResponseBase {
		public:
			std::vector<VappData> data;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};
		class VappRunResponse : public HttpResponseBase {
		public:
			utility::string_t data;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VMInstanceData : public JsonDataBase {
		public:
			int id;         // 序号
			int cpu;        // CPU核数量
			int memory;     // 内存大小
			int diskSize;   // 硬盘大小
			int vmsnId;     // 虚拟机连接Id
			int serverPort; // 连接服务器端口
			int turnPort;   // 转发服务器端口
			int stunPort;   // 穿透服务器端口
			int serverDomainPort;	// tcp 推流服务器端口
			int frameWidth;
			int frameHeight;
			int statusValue; // 状态value
			uint64_t createTime;
			utility::string_t status;
			utility::string_t vmName;
			utility::string_t vmUuid;
			utility::string_t innerIP1;
			utility::string_t description;
			utility::string_t hostIp;
			utility::string_t serverAddress;    // 连接服务器地址
			utility::string_t turnAddress;      // 转发服务器地址
			utility::string_t stunAddress;      // 穿透服务器地址
			utility::string_t serverDomainUrl;	// tcp 推流服务器地址
			utility::string_t instanceKey;      // 实例key

			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
		};

		class UserTokenResponse : public HttpResponseBase {
		public:
			utility::string_t data;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VappFirstConnectResponse : public HttpResponseBase {
		public:
			utility::string_t data;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};


		class VappCloseResponse : public HttpResponseBase {
		public:
			utility::string_t data;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VappHeartbeatResponse : public HttpResponseBase {
		public:
			utility::string_t data;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VappDetailResponse : public HttpResponseDetailBase {
		public:
			utility::string_t data;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VappConnectResponse : public HttpResponseBase {
		public:
			VappInstanceData data;
			virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class ConnectServerStatusResponse : public HttpResponseBase {
		};

	}
}
#else
#include <string>
#include <vector>

namespace gwecom {
	namespace app {

		using namespace std;

		class JsonDataBase {
		public:
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) = 0;
		};

		class HttpResponseBase : public JsonDataBase {
		public:
			int                 tokenStatus;
			int                 code;
			int                 apiCode;
			std::string   message;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
		};

		class HttpResponseDetailBase : public JsonDataBase {
		public:
			int                 code;
			int                 apiCode;
			std::string   message;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
		};

		class VappData : public JsonDataBase {
		public:
			std::string name;
			std::string iconSrc;
			std::string appKey;
			std::string instanceKey;
			std::string runningTimes;

			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
		};
		class VappInstanceData : public JsonDataBase {
		public:
			VappInstanceData() {
				bUseSSL = 0;
				frameWidth = 0;
				frameHeight = 0;
				useWebrtc = 0;
				useTurnServer = 0;
				turnServerIP = "";
				turnServerPort = 0;
				instanceKey = "";
				serverDomainUrl = "";
				serverDomainPort = 0;
				controlImgUrl = "";
				screenDirect = 0;
				connectType = 0;
				accountType = 0;
				janusIP = "";
				janusPort = 0;
				iceServerInfo = "";
				videoCodec = 0;
			}
			bool bUseSSL;
			int frameWidth;
			int frameHeight;
			int useWebrtc;
			int useTurnServer;
			std::string turnServerIP;
			int turnServerPort;
			std::string instanceKey;		//通过后台获取到的实例ID，如果为空，那么表示是直连版本
			std::string serverDomainUrl;	// tcp 推流服务器地址
			int serverDomainPort;	// tcp 推流服务器端口
			std::string controlImgUrl;
			int screenDirect;		// 0:横屏, 1:竖屏
			int connectType;			// 连接方式（0.应用程序1.桌面）
			int accountType;		// 游戏账号：1steam账号2暴雪
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
			//webrtc 连接信息
			std::string janusIP;    // janus http address
			int janusPort;  // janus http port
			std::string iceServerInfo;

			int videoCodec;//视频编码格式 0:H264 1:H265
			//end webrtc
		};
		class VappResponse : public HttpResponseBase {
		public:
			std::vector<VappData> data;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};
		class VappRunResponse : public HttpResponseBase {
		public:
			std::string data;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VMInstanceData : public JsonDataBase {
		public:
			int id;         // 序号
			int cpu;        // CPU核数量
			int memory;     // 内存大小
			int diskSize;   // 硬盘大小
			int vmsnId;     // 虚拟机连接Id
			int serverPort; // 连接服务器端口
			int turnPort;   // 转发服务器端口
			int stunPort;   // 穿透服务器端口
			int serverDomainPort;	// tcp 推流服务器端口
			int frameWidth;
			int frameHeight;
			int statusValue; // 状态value
			uint64_t createTime;
			std::string status;
			std::string vmName;
			std::string vmUuid;
			std::string innerIP1;
			std::string description;
			std::string hostIp;
			std::string serverAddress;    // 连接服务器地址
			std::string turnAddress;      // 转发服务器地址
			std::string stunAddress;      // 穿透服务器地址
			std::string serverDomainUrl;	// tcp 推流服务器地址
			std::string instanceKey;      // 实例key

			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception);
		};

		class UserTokenResponse : public HttpResponseBase {
		public:
			std::string data;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VappFirstConnectResponse : public HttpResponseBase {
		public:
			std::string data;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};


		class VappCloseResponse : public HttpResponseBase {
		public:
			std::string data;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VappHeartbeatResponse : public HttpResponseBase {
		public:
			std::string data;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VappDetailResponse : public HttpResponseDetailBase {
		public:
			std::string data;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class VappConnectResponse : public HttpResponseBase {
		public:
			VappInstanceData data;
			//virtual void initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) override;
		};

		class ConnectServerStatusResponse : public HttpResponseBase {
		};

	}
}
#endif
#endif
