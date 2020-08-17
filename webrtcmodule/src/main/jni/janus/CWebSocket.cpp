#include "CWebSocket.h" 
#define DEBUG_WEBRTC
#ifdef DEBUG_WEBRTC
extern std::string g_strCacertFilePath;
static CWebSocket* OneWebSocket = NULL;

int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	struct session_data *data = (struct session_data *) user;
	/*CWebSocket* websocket = NULL;
	if (data)
		websocket = (CWebSocket*)data->user;*/
	char* sdata;
	std::string rcvdata;
	std::string senddata;
	//char prntstr[1024] = { 0 };
	//sprintf(prntstr, "websocket call back==================== reason:%d\n", reason);
	//OutputDebugStringA(prntstr);
	switch (reason) {
	case LWS_CALLBACK_CLIENT_ESTABLISHED:   // ���ӵ���������Ļص�
		lwsl_notice("Connected to server ok!\n");
		OneWebSocket->onConnected();
		break;
	case LWS_CALLBACK_CLIENT_CLOSED:
		lwsl_notice("Disconnected to server!\n");
		OneWebSocket->onDisConnectd();
		break;
	case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
		lwsl_notice("Conncted Error to server!\n");
		OneWebSocket->onError();
		break;
	case LWS_CALLBACK_CLIENT_RECEIVE:       // ���յ����������ݺ�Ļص�������Ϊin���䳤��Ϊlen
		sdata = (char*)in;
		rcvdata = sdata;
		//if (!OneWebSocket->m_waitSend)
		WriteLogNetWork("recive data:%s", sdata);
		OneWebSocket->onRecieve(rcvdata);
		//lwsl_notice("Rx: %s size:%d\n", sdata, len);
		
		//lwsl_notice("Rx: %s\n", (sdata);
		break;
	case LWS_CALLBACK_CLIENT_WRITEABLE:     // ���˿ͻ��˿��Է�������ʱ�Ļص�
		senddata = OneWebSocket->getSendData();
		if (senddata.size() > 0) {
			data->len = senddata.size();
			const char* snddata = senddata.c_str();
			WriteLogNetWork("websocket send data:%s", snddata);
			memcpy(data->buf + LWS_PRE, snddata, data->len);
			lws_write(wsi, &data->buf[LWS_PRE], data->len, LWS_WRITE_BINARY);
		}
		break;
	}
	return 0;
}

struct lws_protocols protocols[] = {
	{
		//Э�����ƣ�Э��ص������ջ�������С 
		"janus-protocol", callback, sizeof(struct session_data), MAX_PAYLOAD_SIZE,
	},
	//{
	////Э�����ƣ�Э��ص������ջ�������С
	////"ws", callback, sizeof(struct session_data), MAX_PAYLOAD_SIZE,
	//"janus-protocol", callback, sizeof(struct session_data), MAX_PAYLOAD_SIZE,
	//},
	{
		NULL, NULL,   0 // ���һ��Ԫ�ع̶�Ϊ�˸�ʽ
	}
};

void onTimerWebSocketReConnect(void* user) {
	CWebSocket* websocket = (CWebSocket*)user;
	websocket->connect();
}

CWebSocket::CWebSocket() 
{
	OneWebSocket = this;
	m_close = false;
	//m_connectSemp = new boost::interprocess::interprocess_semaphore(0);
	//m_sendSemp = new boost::interprocess::interprocess_semaphore(0);
	m_connectTimer = new ATimer<>(this);
	m_connectTimer->setSingleShot(true);
	m_connectTimer->bind(onTimerWebSocketReConnect);
	m_alive = false;
	
}

CWebSocket::~CWebSocket() {
	m_close = true;
	m_alive = false;
	m_connectTimer->stop();
	m_thread.join();
	//delete m_sendSemp;
	//delete m_connectSemp;
}

bool CWebSocket::initWebSocket(const char* HostPath, WSDataCB wsCB, void* user)
{
	m_wsCB = wsCB;
	m_user = user;
	WriteLogNetWork("web socket init hotsInfo:%s", HostPath);
	const char *prot, *path, *address;
	
	lws_parse_uri((char*)HostPath, &prot, &address, &m_hostPort, &path);
	if (!strcmp(prot, "http") || !strcmp(prot, "ws"))
		m_jwst = jwstNoSsl;
	if (!strcmp(prot, "https") || !strcmp(prot, "wss"))
		m_jwst = jwstSsl;
	m_hostIP = address;
	//m_hostPort = Port;
	m_hostPath = path;

	
	return connect(); 
}

bool CWebSocket::sendData(std::string data)
{
	//std::string result = "";
	
	if (!m_connectState) {
		return false;
	}
	//m_waitSend = true;
	//m_recved = "";
	{
		std::lock_guard<std::mutex> lock(m_sendLock);
		m_sendList.push_back(data);
	}
	
	////WriteLogNetWork("web socket send begin");
	//m_sendSemp->wait();
	////WriteLogNetWork("web socket send: %s recieve:%s", data.c_str(), m_recved.c_str());
	//m_waitSend = false;
	//return m_recved; 
	return true;
}

void CWebSocket::onConnected()
{
	WriteLogNetWork("web socket connnceted");
	m_connectState = true; 
	m_wsCB(wsdtNotice, NULL, wsstConnected, m_user);
}

void CWebSocket::onError()
{
	WriteLogNetWork("web socket onError");
	m_connectState = false; 
	m_wsCB(wsdtNotice, NULL, wsstError, m_user);
}

void CWebSocket::onDisConnectd()
{
	WriteLogNetWork("web socket disconnect");
	m_connectState = false; 
	m_wsCB(wsdtNotice, NULL, wsstDisconnect, m_user);
}

void CWebSocket::onRecieve(std::string srcv)
{
	//m_recved = srcv;
	m_wsCB(wsdtData, srcv.c_str(), srcv.size(), m_user);
	//if (m_waitSend)
		//m_sendSemp->post();
}

void CWebSocket::startReconect()
{
	//m_connectTimer.stop();
	if (!m_close)
		m_connectTimer->start(3000);
}

std::string CWebSocket::getSendData()
{	
	std::lock_guard<std::mutex> lock(m_sendLock);
	std::string senddata = "";
	if (m_sendList.empty())
		return senddata;
	
	senddata = m_sendList.front();
	m_sendList.pop_front();
	return senddata;
}

void CWebSocket::Process()
{

	//struct lws *wsi = lws_client_connect_via_info(&conn_info);
	while (m_alive)
	{
		// ִ��һ���¼�ѭ����Poll������ȴ�1000����
		lws_service(context, 500);
		/**
		 * ����ĵ��õ������ǣ������ӿ��Խ���������ʱ������һ��WRITEABLE�¼��ص�
		 * ���������ں�̨��������ʱ�������ܽ����µ�����д����������WRITEABLE�¼��ص�����ִ��
		 */
		 lws_callback_on_writable(wsi);
#ifdef _WIN32
		 Sleep(1);
#endif
#ifdef ANDROID
		 usleep(100);
#endif
	}
	// ���������Ķ��� 
	lws_context_destroy(context);
}

bool CWebSocket::connect()
{
	if (m_alive) {
		m_alive = false;
		m_thread.join();
	}
	
	struct lws_context_creation_info ctx_info = { 0 };
	ctx_info.port = CONTEXT_PORT_NO_LISTEN;
	ctx_info.iface = NULL;
	ctx_info.protocols = protocols;
	ctx_info.gid = -1;
	ctx_info.uid = -1;

	//ssl֧�֣�ָ��CA֤�顢�ͻ���֤�鼰˽Կ·������ssl֧�֣�
	//ctx_info.ssl_ca_filepath = "../ca/ca-cert.pem";
	//ctx_info.ssl_cert_filepath = "./client-cert.pem";
	//ctx_info.ssl_private_key_filepath = "./client-key.pem";

	// ����һ��WebSocket������
#ifdef _WIN32
	std::string pempath = GetAPPPath() + "\\cacert.dat";
#endif
#ifdef ANDROID
	std::string pempath =  g_strCacertFilePath;;
#endif
	switch (m_jwst)
	{
	case jwstNoSsl:

		break;
	case jwstSsl:

		ctx_info.ssl_ca_filepath = pempath.c_str();

		//ctx_info.server_ssl_ca_mem = (void*)Server_SSL_CA_MEM;
		//ctx_info.server_ssl_ca_mem_len = fsize;
		ctx_info.options |= LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT;
		break;
	default:
		break;
	}


	context = lws_create_context(&ctx_info);

	char addr_port[256] = { 0 };
	sprintf(addr_port, "%s:%u", m_hostIP.c_str(), m_hostPort & 65535);

	// �ͻ������Ӳ���
	struct lws_client_connect_info conn_info = { 0 };
	conn_info.context = context;
	conn_info.address = m_hostIP.c_str();
	conn_info.port = m_hostPort;
	switch (m_jwst)
	{
	case jwstNoSsl:
		conn_info.ssl_connection = 0;
		break;
	case jwstSsl:

		conn_info.ssl_connection = 1;
		break;
	default:
		break;
	}

	conn_info.path = m_hostPath.c_str();
	conn_info.host = addr_port;
	conn_info.origin = addr_port;
	conn_info.protocol = protocols[0].name;

	wsi = lws_client_connect_via_info(&conn_info); 
	m_alive = true;
	m_connectState = false;
	m_thread = std::thread(&CWebSocket::Process, this);
	//m_waitSend = false;
	wait();
	
	return m_connectState;
}
//2019.5.21 �޸�Ϊ�첽����,״̬֪ͨ���ⲿ
void CWebSocket::wait()
{
	return;
	//m_connectSemp->wait();
}

void CWebSocket::post()
{
	return;
	//����ڵȴ����ͽ����;�������ӶϿ�����Ҫȡ���ȴ�
	
	/*if (m_waitSend)
		m_sendSemp->post();*/
	//m_connectSemp->post();
	/*if (!m_close)
		m_connectTimer->start(3000);*/
}
#endif


