#include "httpmodelsrest.h"
#if UseCppRest
namespace gwecom {
	namespace app {

		void HttpResponseBase::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			auto string = jsonValue.to_string();
			code = get_json_integer(jsonValue, _XPLATSTR("code"));
			apiCode = get_json_integer(jsonValue, _XPLATSTR("apiCode"));
			tokenStatus = get_json_integer(jsonValue, _XPLATSTR("tokenStatus"));
			message = get_json_string(jsonValue, _XPLATSTR("message"));
		}
		void HttpResponseDetailBase::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			auto string = jsonValue.to_string();
			code = get_json_integer(jsonValue, _XPLATSTR("code"));
			apiCode = get_json_integer(jsonValue, _XPLATSTR("apiCode"));
			message = get_json_string(jsonValue, _XPLATSTR("message"));
		}

		void VMInstanceData::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			id = get_json_integer(jsonValue, _XPLATSTR("id"));
			cpu = get_json_integer(jsonValue, _XPLATSTR("cpu"));
			memory = get_json_integer(jsonValue, _XPLATSTR("memory"));
			diskSize = get_json_integer(jsonValue, _XPLATSTR("diskSize"));
			vmsnId = get_json_integer(jsonValue, _XPLATSTR("vmsnId"));
			serverPort = get_json_integer(jsonValue, _XPLATSTR("serverPort"));
			turnPort = get_json_integer(jsonValue, _XPLATSTR("turnPort"));
			stunPort = get_json_integer(jsonValue, _XPLATSTR("stunPort"));
			serverDomainPort = get_json_integer(jsonValue, _XPLATSTR("serverDomainPort"));
			frameWidth = get_json_integer(jsonValue, _XPLATSTR("frameWidth"));
			frameHeight = get_json_integer(jsonValue, _XPLATSTR("frameHeight"));
			createTime = get_json_integer64(jsonValue, _XPLATSTR("createTime"));
			status = get_json_string(jsonValue, _XPLATSTR("status"));
			statusValue = get_json_integer(jsonValue, _XPLATSTR("statusValue"));
			vmName = get_json_string(jsonValue, _XPLATSTR("vmName"));
			vmUuid = get_json_string(jsonValue, _XPLATSTR("vmUuid"));
			innerIP1 = get_json_string(jsonValue, _XPLATSTR("innerIP1"));
			description = get_json_string(jsonValue, _XPLATSTR("description"));
			hostIp = get_json_string(jsonValue, _XPLATSTR("hostIp"));
			serverAddress = get_json_string(jsonValue, _XPLATSTR("serverAddress"));
			turnAddress = get_json_string(jsonValue, _XPLATSTR("turnAddress"));
			stunAddress = get_json_string(jsonValue, _XPLATSTR("stunAddress"));
			instanceKey = get_json_string(jsonValue, _XPLATSTR("instanceKey"));
			serverDomainUrl = get_json_string(jsonValue, _XPLATSTR("serverDomainUrl"));
		}
		void VappData::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			name = get_json_string(jsonValue, _XPLATSTR("name"));
			iconSrc = get_json_string(jsonValue, _XPLATSTR("iconsrc"));
			appKey = get_json_string(jsonValue, _XPLATSTR("appkey"));
			instanceKey = get_json_string(jsonValue, _XPLATSTR("instancekey"));
			runningTimes = get_json_string(jsonValue, _XPLATSTR("runningtimes"));
		}
		void VappInstanceData::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			serverPort = get_json_integer(jsonValue, _XPLATSTR("serverPort"));
			turnPort = get_json_integer(jsonValue, _XPLATSTR("turnPort"));
			stunPort = get_json_integer(jsonValue, _XPLATSTR("stunPort"));
			serverDomainPort = get_json_integer(jsonValue, _XPLATSTR("serverDomainPort"));
			frameWidth = get_json_integer(jsonValue, _XPLATSTR("frameWidth"));
			frameHeight = get_json_integer(jsonValue, _XPLATSTR("frameHeight"));

			serverAddress = get_json_string(jsonValue, _XPLATSTR("serverAddress"));
			turnAddress = get_json_string(jsonValue, _XPLATSTR("turnAddress"));
			stunAddress = get_json_string(jsonValue, _XPLATSTR("stunAddress"));
			serverDomainUrl = get_json_string(jsonValue, _XPLATSTR("serverDomainUrl"));
			controlImgUrl = get_json_string(jsonValue, _XPLATSTR("controlImgUrl"));
			screenDirect = get_json_integer(jsonValue, _XPLATSTR("screenDirect"));
		}

		void VappResponse::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			HttpResponseBase::initWithJsonValue(jsonValue);
			data.clear();

			try {
				auto jsonData = jsonValue.at(_XPLATSTR("data")).as_array();
				for (auto json : jsonData) {
					VappData vappData;
					vappData.initWithJsonValue(json);
					data.push_back(vappData);
				}
			}
			catch (const std::exception& e) {
				const char* eString = e.what();
				wcout << eString << std::endl;
			}
		}

		void UserTokenResponse::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			HttpResponseBase::initWithJsonValue(jsonValue);
			data.clear();
			try {
				data = get_json_string(jsonValue, _XPLATSTR("data"));
			}
			catch (const std::exception& e) {
				const char* eString = e.what();
				wcout << eString << std::endl;
			}
		}

		void VappFirstConnectResponse::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			HttpResponseBase::initWithJsonValue(jsonValue);
			data.clear();
			try {
				data = get_json_string(jsonValue, _XPLATSTR("data"));
			}
			catch (const std::exception& e) {
				const char* eString = e.what();
				wcout << eString << std::endl;
			}
		}


		void VappCloseResponse::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			HttpResponseBase::initWithJsonValue(jsonValue);
			data.clear();
			try {
				data = get_json_string(jsonValue, _XPLATSTR("data"));
			}
			catch (const std::exception& e) {
				const char* eString = e.what();
				wcout << eString << std::endl;
			}
		}


		void VappHeartbeatResponse::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			HttpResponseBase::initWithJsonValue(jsonValue);
			data.clear();
			try {
				data = get_json_string(jsonValue, _XPLATSTR("data"));
			}
			catch (const std::exception& e) {
				const char* eString = e.what();
				wcout << eString << std::endl;
			}
		}

		void VappDetailResponse::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			HttpResponseDetailBase::initWithJsonValue(jsonValue);
			data.clear();
			try {
				data = get_json_string(jsonValue, _XPLATSTR("data"));
			}
			catch (const std::exception& e) {
				const char* eString = e.what();
				wcout << eString << std::endl;
			}
		}

		void VappConnectResponse::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			HttpResponseBase::initWithJsonValue(jsonValue);
			try {
				data.initWithJsonValue(jsonValue.at(_XPLATSTR("data")));
			}
			catch (const std::exception& e) {
				const char* eString = e.what();
				wcout << eString << std::endl;
			}
		}
		void VappRunResponse::initWithJsonValue(web::json::value jsonValue) throw (web::json::json_exception) {
			HttpResponseBase::initWithJsonValue(jsonValue);
			data.clear();
			try {
				data = get_json_string(jsonValue, _XPLATSTR("data"));
			}
			catch (const std::exception& e) {
				const char* eString = e.what();
				wcout << eString << std::endl;
			}
		}

	}
}
#endif