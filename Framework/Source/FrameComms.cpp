#include "../Header/FrameComms.h"


FrameCommunications::FrameCommunications()
	: WsaPort("8008"),
	IsBound(false),
	ConnectSocket(INVALID_SOCKET)
	
{
#ifdef _WIN32
	WSADATA wsa;
	WSAStartup(MAKEWORD(2, 2), &wsa);
#endif



}

FrameCommunications::~FrameCommunications()
{
	if (IsBound)
	{
		CloseSocket(ConnectSocket);
	}

	Cleanup();

}

int FrameCommunications::ConnectToHost(std::string ip)
{
	struct addrinfo *res = nullptr,
		*ptr = nullptr,
		hints;

	SecureZeroMemory(&hints, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	auto err = getaddrinfo(ip.c_str(), WsaPort.c_str(), &hints, &res);
	if(WAS_FAILURE(err))
	{
		Cleanup();
		return 1;
	}

	// first connection
	ptr = res;

	ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
	if(IS_INVALID_SOCKET(ConnectSocket))
	{
		freeaddrinfo(res);
		Cleanup();
		return 1;
	}

	err = connect(ConnectSocket, ptr->ai_addr, int(ptr->ai_addrlen));
	if(WAS_FAILURE(err))
	{
		CloseSocket(ConnectSocket);
		//Connection = INVALID_SOCKET;
		freeaddrinfo(res);
		Cleanup();
		return 1;
	}

	freeaddrinfo(res);

	// Launch process thread
	NetThread = std::make_unique<std::thread>(&FrameCommunications::NetworkThreadRt, this);

	IsBound = true;
	return 0;
}

void FrameCommunications::NetworkThreadRt()
{
	const auto recvBuffer = new char[4096];
	int result;
	do
	{
		result = recv(ConnectSocket, recvBuffer, 4096, 0);
	} while (result > 0);

	delete[] recvBuffer;
}

void FrameCommunications::Cleanup()
{
#ifdef _WIN32
	WSACleanup();
#endif
}

void FrameCommunications::CloseSocket(const crySocket sock)
{
#ifdef _WIN32
	const auto status = shutdown(sock, SD_BOTH);
	if (WAS_SUCCESS(status)) closesocket(sock);
#else
	const auto status = shutdown(sock, SHUT_RDWR);
	if (WAS_SUCCESS(status)) close(sock);
#endif
}
