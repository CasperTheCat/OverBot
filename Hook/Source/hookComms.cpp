#include "../Header/hookComms.h"
#include "../Header/utils.h"
#include <iostream>
#include <minwinbase.h>
//#include <ws2def.h>

HookCommunications::HookCommunications() :
    WsaPort("8008"),
    bIsBound(false)
{
	// Launch Thread
	NetThread = std::make_unique<std::thread>(&HookCommunications::NetworkThreadRt, this);
}

HookCommunications::~HookCommunications()
{
    // Delete open connections

    // Delete the socket
	closesocket(ServerSocket);
	WSACleanup();

	if(NetThread->joinable()) NetThread->join();
}

[[nodiscard]]
bool HookCommunications::CreateSocketAndBind()
{
	// Windows Socket returns 0 on success...
	if (!WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		return false;
	}

	addrinfo *res = nullptr, *ptr = nullptr, hints;

	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = IPPROTO_TCP; // USING TCP FOR NOW TODO CHANGE
	hints.ai_flags = AI_PASSIVE;

	// Get Local addr
	auto err = getaddrinfo(nullptr, WsaPort.c_str(), &hints, &res);
	if (err)
	{
		WSACleanup();
		return false;
	}

	
	ServerSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (ServerSocket == INVALID_SOCKET) 
	{
		freeaddrinfo(res);
		WSACleanup();
		return false;
	}

	err = bind(ServerSocket, res->ai_addr, int(res->ai_addrlen));
	if(err == SOCKET_ERROR)
	{
		freeaddrinfo(res);
		closesocket(ServerSocket);
		WSACleanup();
		return false;
	}

	bIsBound = true;
    freeaddrinfo(res);
	return true;
}

void HookCommunications::Signal_EndFrame()
{
	// Append Signal to Thread Queue
	// Signal the network thread. It's able to send now.
}

void HookCommunications::NetworkThreadRt()
{
	// Network Thread Logic
	// This loops infinitely until we recieve a kill from the host
}

[[nodiscard]]
FQueueMessage HookCommunications::ThreadRt_GetNextMessage()
{
	FQueueMessage dummy;
	dummy.type = EQueueMessageType::NetThreadStop;
	return dummy;
}

//////////////////////////////////////////////////
// Notifiers - Supports
//
    /**
     * Notify of any source of damage boost
     */
void HookCommunications::Notify_DamageBoost()
{

}

    /**
     * Notify of incoming healing
     */
void HookCommunications::Notify_Healing()
{

}

    /**
     * Notify of the inability to be healed
     */
void HookCommunications::Notify_AntiHealing()
{

}


//////////////////////////////////////////////////
// Notifiers - Damage
//
    /**
     * Notify that we hit something
     */
void HookCommunications::Notify_DamageDealt()
{

}

    /**
     * Notify that we took damage
     */
void HookCommunications::Notify_DamageTaken()
{

}

//////////////////////////////////////////////////
// Notifiers - Health
//
    /**
     * Notify that we hit critical HP
     */
void HookCommunications::Notify_HealthCritical()
{

}

    /**
     * Notify that our health changed 
     */
void HookCommunications::Notify_HealthChanged(int newHealth)
{

}
