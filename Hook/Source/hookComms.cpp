#include "../Header/hookComms.h"
#include "../Header/utils.h"
#include <iostream>
#include <minwinbase.h>
//#include <ws2def.h>

HookCommunications::HookCommunications() :
	WsaPort("8008"),
	bIsBound(false), 
	bCanPushQueue(false),
	ServerSocket(INVALID_SOCKET),
	ClientSocket(INVALID_SOCKET)
{
}

HookCommunications::~HookCommunications()
{
    // Delete open connections

    // Delete the socket
	if (bIsBound)
	{
		closesocket(ServerSocket);
		WSACleanup();
	}

	if(NetThread->joinable()) NetThread->join();
}

[[nodiscard]]
bool HookCommunications::CreateSocketAndBind()
{
	if (bIsBound) return true;

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

	// Launch Thread
	NetThread = std::make_unique<std::thread>(&HookCommunications::NetworkThreadRt, this);

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

	if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) {
		return; // Exit
	}

	// Wait for connection
	ClientSocket = accept(ServerSocket, nullptr, nullptr);

	if(ClientSocket == INVALID_SOCKET)
	{
		return; // Returning here does not take the DLL out
	}

	// We have a client. We can allow writes to the queue now
	{
		std::lock_guard<std::mutex> lock(netMutex);
		bCanPushQueue = true;
	}

	// ReSharper disable once CppJoinDeclarationAndAssignment
	EQueueMessageType MsgType;
	bool ThreadRun = true;

	while(ThreadRun)
	{
		//
		MsgType = ThreadRt_BlockForNextMessage();

		switch(MsgType)
		{
		case EQueueMessageType::NetThreadStart: break;
		case EQueueMessageType::NetThreadStop: 
			ThreadRun = false;
			break;
		case EQueueMessageType::Signal_EndFrame: break;
		case EQueueMessageType::Notify_DamageBoost: break;
		case EQueueMessageType::Notify_Healing: break;
		case EQueueMessageType::Notify_AntiHealing: break;
		case EQueueMessageType::TOTAL_MESSAGE_TYPES: break;
		default: ;
		}
	}
}

[[nodiscard]]
EQueueMessageType HookCommunications::ThreadRt_GetNextMessage()
{
	// Lock
	std::lock_guard<std::mutex> lock(netMutex);
	const auto t = NetThreadMessageQueue.front();
	NetThreadMessageQueue.pop();
	return t;
}

bool HookCommunications::ThreadRt_IsQueueEmpty()
{
	return true;
}

EQueueMessageType HookCommunications::ThreadRt_BlockForNextMessage()
{
	EQueueMessageType retMessage;
	while(true)
	{
		// Lock
		std::unique_lock<std::mutex> lock(netMutex);

		if (NetThreadMessageQueue.empty())
		{
			// Unlock here
			lock.unlock();
			Sleep(50); // We can be nice and not spam. If the queue is not empty
					   // we will run at max speed anyway
			continue;
		}

		// First element
		retMessage = NetThreadMessageQueue.front();

		// Remove last element
		NetThreadMessageQueue.pop();

		break;
	}

	return retMessage;
}

void HookCommunications::ThreadRt_TransmitToClient(EQueueMessageType Message)
{
	// We transmit to client here
	std::string txString;
	switch (Message)
	{
		case EQueueMessageType::NetThreadStart: break;
		case EQueueMessageType::NetThreadStop: break;
		case EQueueMessageType::Signal_EndFrame: break;
		case EQueueMessageType::Notify_DamageBoost:
			txString = "SGNL_DmgBst";
			break;
		case EQueueMessageType::Notify_Healing: break;
		case EQueueMessageType::Notify_AntiHealing: break;
		case EQueueMessageType::TOTAL_MESSAGE_TYPES: break;
		default: ;
	}

	// TX
	send(ClientSocket, txString.c_str(), strlen(txString.c_str()), 0);
}

void HookCommunications::MainRt_PushMessage(const EQueueMessageType Message)
{
	// Lock
	std::lock_guard<std::mutex> lock(netMutex);
	if(bCanPushQueue) NetThreadMessageQueue.push(Message);
}

//////////////////////////////////////////////////
// Notifiers - Supports
//
    /**
     * Notify of any source of damage boost
     */
void HookCommunications::Notify_DamageBoost()
{
	
	MainRt_PushMessage(EQueueMessageType::Notify_DamageBoost);
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
