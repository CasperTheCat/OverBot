#include "../Header/hookComms.h"
#include "../Header/utils.h"
#include <iostream>
#include <minwinbase.h>
//#include <ws2def.h>

HookCommunications::HookCommunications() :
    wsaPort("8008"),
    res(nullptr),
    bIsBound(false)
{

}

HookCommunications::~HookCommunications()
{
    // Delete open connections

    // Delete the socket
	closesocket(ServerSocket);
	WSACleanup();
}

[[nodiscard]]
bool HookCommunications::CreateSocketAndBind()
{
	// Windows Socket returns 0 on success...
	if (!WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		this->Event << LOGERR("WSAStartup Failed.") << std::endl;
		return false;
	}

	addrinfo *res = nullptr, *ptr = nullptr, hints;

	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = IPPROTO_TCP; // USING TCP FOR NOW TODO CHANGE
	hints.ai_flags = AI_PASSIVE;

	// Get Local addr
	auto err = getaddrinfo(nullptr, wsaPort.c_str(), &hints, &res);
	if (err)
	{
		Event << LOGERR("Unable to get address info.") << std::endl;
		WSACleanup();
		return false;
	}

	SOCKET ServerSocket = INVALID_SOCKET;
	ServerSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (ServerSocket == INVALID_SOCKET) 
	{
		Event << LOGERR("Error with Socket ") << WSAGetLastError() << std::endl;
		freeaddrinfo(res);
		WSACleanup();
		return false;
	}

	err = bind(ServerSocket, res->ai_addr, int(res->ai_addrlen));
	if(err == SOCKET_ERROR)
	{
		Event << LOGERR("Unable to bind") << std::endl;
		freeaddrinfo(res);
		closesocket(ServerSocket);
		WSACleanup();
		return false;
	}

	bIsBound = true;
    freeaddrinfo(res);
	return true;
}

void HookCommunications::Notify_DamageBoost()
{
    // NOTHING
}
