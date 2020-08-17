#include "WebrtcDef.h"


const char kAudioLabel[] = "audio_label";
const char kVideoLabel[] = "video_label";
const char kStreamLabel[] = "stream_label";
#ifdef _WIN32
#include "time.h"
#include "windows.h"
void debugwebrtcstr(const char* str) {
	char buf[1024];
	sprintf(buf, "%s", str);
	for (int i = 0; i < strlen(buf); i++) {

		/*if (buf[i] == '\n') {
			buf[i] = ' ';
		}
		if (buf[i] == '\r') {
			buf[i] = ' ';
		}*/
	}
	OutputDebugStringA(buf);
}

void logwebfunction(const std::string& function,
	const std::string& file,
	int line) {
	time_t now = time(nullptr);
	struct tm* current_time = localtime(&now);
	char buf[1024];
	strftime(buf, sizeof(buf), " %Y-%m-%d %H:%M:%S", current_time);
	std::string logtxt = std::string("[webrtcclient ") + buf + "] " +
		/*" file:" + file*/ +" function:" + function +
		" lines:" + std::to_string(line) + "\n";

	// qDebug() << QString::fromLocal8Bit(logtxt);
	debugwebrtcstr(logtxt.c_str());
}
void logwebstr(const std::string& function,
	const std::string& file,
	int line, char* str) {
	time_t now = time(nullptr);
	struct tm* current_time = localtime(&now);
	char buf[1024];
	strftime(buf, sizeof(buf), " %Y-%m-%d %H:%M:%S", current_time);
	std::string logtxt = std::string("[webrtcclient ") + buf + "] " +
		/*" file:" + file*/ +" function:" + function +
		" lines:" + std::to_string(line) + str + "\n";

	// qDebug() << QString::fromLocal8Bit(logtxt);
	debugwebrtcstr(logtxt.c_str());
}

#else 
void logwebfunction(const std::string& function,
	const std::string& file,
	int line) {

}

void logwebstr(const std::string& function,
	const std::string& file,
	int line, char* str) {

}
#endif