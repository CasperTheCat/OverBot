#include "../Header/hookComms.h"
#include "../Header/utils.h"
#include <iostream>
#include <minwinbase.h>
#include <cassert>
//#include <ws2def.h>

HookCommunications::HookCommunications() :
	WsaPort("8008"),
	bIsBound(false), 
	bCanPushQueue(false),
	ServerSocket(INVALID_SOCKET),
	ClientSocket(INVALID_SOCKET)
{
	Event.open("Net.log");
	Event << LOG("Initialising") << std::endl;
}

HookCommunications::~HookCommunications()
{
    // Delete open connections
    // Delete the socket
	if (bIsBound)
	{
		const auto stat = shutdown(ServerSocket, SD_BOTH);
		if (WAS_SUCCESS(stat)) closesocket(ServerSocket);
		WSACleanup();
		ThreadKill = true;
		if (NetThread->joinable()) NetThread->join();
		bIsBound = false;
	}

	Event.close();
}

[[nodiscard]]
int HookCommunications::CreateSocketAndBind()
{
	Event << "Binding" << std::endl;
	if (bIsBound) return EXIT_SUCCESS; // Already up
	Event << "Ice" << std::endl;

	// Windows Socket returns 0 on success...
	if (WAS_FAILURE(WSAStartup(MAKEWORD(2, 2), &wsaData)))
	{
		Event << "Startup Failed" << WSAGetLastError() << std::endl;
		return 1;
	}
	
	addrinfo *res = nullptr, *ptr = nullptr, hints;

	SecureZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP; // USING TCP FOR NOW TODO CHANGE
	hints.ai_flags = AI_PASSIVE;

	// Get Local addr
	auto err = getaddrinfo(nullptr, WsaPort.c_str(), &hints, &res);
	if (WAS_FAILURE(err))
	{
		Event << "addrinfo issue" << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	Event << "addrs" << std::endl;
	
	ServerSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (ServerSocket == INVALID_SOCKET) 
	{
		freeaddrinfo(res);
		WSACleanup();
		return 1;
	}
	Event << "sock" << std::endl;
	err = bind(ServerSocket, res->ai_addr, int(res->ai_addrlen));
	if(err == SOCKET_ERROR)
	{
		freeaddrinfo(res);
		closesocket(ServerSocket);
		WSACleanup();
		return 1;
	}
	Event << "bind" << std::endl;
	// Launch Thread
	ThreadKill = false;
	NetThread = std::make_unique<std::thread>(&HookCommunications::NetworkThreadRt, this);

	bIsBound = true;
    freeaddrinfo(res);
	return EXIT_SUCCESS;
}

void HookCommunications::CleanAndReset()
{
	if (bIsBound)
	{
		const auto stat = shutdown(ServerSocket, SD_BOTH);
		if (WAS_SUCCESS(stat)) closesocket(ServerSocket);
		WSACleanup();
		ThreadKill = true;
		if (NetThread->joinable()) NetThread->join();
		bIsBound = false;
	}


}

void HookCommunications::Signal_EndFrame()
{
	// Append Signal to Thread Queue
	// Signal the network thread. It's able to send now.
	MainRt_PushMessage(EQueueMessageType::Signal_EndFrame);
}

void HookCommunications::NetworkThreadRt()
{
	Event << "nettError" << std::endl;
	// Network Thread Logic
	// This loops infinitely until we recieve a kill from the host

	if (listen(ServerSocket, SOMAXCONN) == SOCKET_ERROR) {
		Event << "listenError" << std::endl;
		return; // Exit
	}

	// Wait for connection
	ClientSocket = accept(ServerSocket, nullptr, nullptr);

	if(ClientSocket == INVALID_SOCKET)
	{
		Event << "accError" << std::endl;
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

	while(ThreadRun && !ThreadKill)
	{
		//
		MsgType = ThreadRt_BlockForNextMessage();

		switch(MsgType)
		{
		case EQueueMessageType::NetThreadStart: break;
		case EQueueMessageType::NetThreadStop: 
			ThreadRun = false;
			break;
		case EQueueMessageType::Signal_EndFrame:
			ThreadRt_TransmitToClient(EQueueMessageType::Signal_EndFrame);
			break;
		case EQueueMessageType::Notify_DamageBoost: break;
		case EQueueMessageType::Notify_Healing: break;
		case EQueueMessageType::Notify_AntiHealing: break;
		case EQueueMessageType::TOTAL_MESSAGE_TYPES: break;
		case EQueueMessageType::NullSignal: break;
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
			//Event << "Out of Messages" << std::endl;
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
	// This is a little ahead of current dev.
	// This is build to enqueue a sending queue that sends on 4KB or EndFrame
	char sendBuffer[4];
	SecureZeroMemory(sendBuffer, 4);
	switch (Message)
	{
		case EQueueMessageType::NetThreadStart: break;
		case EQueueMessageType::NetThreadStop: break;
		case EQueueMessageType::Signal_EndFrame:
			sendBuffer[0] = static_cast<std::underlying_type_t<EQueueMessageType>>(EQueueMessageType::Signal_EndFrame);
			assert(sizeof(std::underlying_type_t<EQueueMessageType>) == sizeof(char));
			break;
		case EQueueMessageType::Notify_DamageBoost:
			break;
		case EQueueMessageType::Notify_Healing: break;
		case EQueueMessageType::Notify_AntiHealing: break;
		case EQueueMessageType::TOTAL_MESSAGE_TYPES: break;
		default: ;
	}

	// TX all 4
	send(ClientSocket, sendBuffer, 4, 0);
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
