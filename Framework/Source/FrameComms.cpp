#include "../Header/FrameComms.h"
#include <iostream>


FrameCommunications::FrameCommunications()
	: IsBound(false),
	ServingPort("8008"),
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
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	auto err = getaddrinfo(ip.c_str(), ServingPort.c_str(), &hints, &res);
	if(WAS_FAILURE(err))
	{
		std::cout << "Error addr" << std::endl;
		Cleanup();
		return 1;
	}

	// first connection
	ptr = res;
	for (ptr = res; ptr != nullptr; ptr = ptr->ai_next)
	{
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (IS_INVALID_SOCKET(ConnectSocket))
		{
			std::cout << "Error sock" << std::endl;
			freeaddrinfo(res);
			Cleanup();
			return 1;
		}

		err = connect(ConnectSocket, ptr->ai_addr, int(ptr->ai_addrlen));
		if (WAS_FAILURE(err))
		{
			std::cout << "Error conn" << std::endl;
			CloseSocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			//freeaddrinfo(res);
			//Cleanup();
			//return 1;
		}
		//break;
	}

	freeaddrinfo(res);

	// Launch process thread
	NetThread = std::make_unique<std::thread>(&FrameCommunications::NetworkThreadRt, this);

	IsBound = true;
	return 0;
}

void FrameCommunications::Run()
{
	if(NetThread->joinable()) NetThread->join();
}

void FrameCommunications::NetworkThreadRt()
{
	EQueueMessageType *castPointer = nullptr;
	const auto recvBuffer = new char[4096];
	// ZeroTheMemory
	SecureZeroMemory(recvBuffer, 4096);

	uint16_t recvs;
	int result;
	do
	{
		result = recv(ConnectSocket, recvBuffer, 4096, 0);
		if (result == 0) 
		{
			printf("Connection closed\n");
			break;
		}
		// make this a string
		//recvBuffer[result] = 0; // Null

		// Cast to uint8_t, the underlying enum type
		castPointer = reinterpret_cast<EQueueMessageType*>(recvBuffer);

		for(recvs = 0; recvs < result; ++recvs)
		{
			// Process each one of the inputs, breaking on nullsignal
			if (castPointer[recvs] == EQueueMessageType::NullSignal) break;
			switch(castPointer[recvs])
			{
				case EQueueMessageType::NetThreadStart: break;
				case EQueueMessageType::NetThreadStop: break;
				case EQueueMessageType::Signal_EndFrame:
					std::cout << "Received Signal for Instanced Draw" << std::endl;
					break;
				case EQueueMessageType::Notify_DamageBoost: break;
				case EQueueMessageType::Notify_Healing: break;
				case EQueueMessageType::Notify_AntiHealing: break;
				case EQueueMessageType::TOTAL_MESSAGE_TYPES: break;
				default: ;
			}
		}

	} while (result > 0);

	delete[] recvBuffer;
}

void FrameCommunications::QueueThreadRt()
{
	return;
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
