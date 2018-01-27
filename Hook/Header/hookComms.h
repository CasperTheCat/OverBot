#pragma once

// Socks
#include <WinSock2.h> // Baremetal Sockets
#include <ws2tcpip.h> // TCPIP Header
#include <queue>
#include <mutex>
#include <fstream>
#include "../../Shared/shared.h"
#pragma comment(lib, "Ws2_32.lib")



class HookCommunications
{
protected:
	std::string WsaPort;
    std::queue<EQueueMessageType> NetThreadMessageQueue;
    std::mutex netMutex;
    std::unique_ptr<std::thread> NetThread;
	// Server Up?
	bool bIsBound;
	bool bCanPushQueue;
	SOCKET ServerSocket;
	SOCKET ClientSocket;
	std::ofstream Event;
	bool ThreadKill;

public:
	/**
	 * Windows Socket Data
	 */
	WSADATA wsaData;


protected:



public:
	HookCommunications();
	~HookCommunications();

	/**
	 * Create Socket and Bind (NO DISCARD)
	 */
	[[nodiscard]]
	int CreateSocketAndBind();

	/**
	 * Cleanup and reset
	 */
	void CleanAndReset();

    /**
     * Signal EndFrame 
     */
    void Signal_EndFrame();

//////////////////////////////////////////////////
// Thread Runtimes
//
private:
    /**
     * Network Thread Runtime 
     */
    void NetworkThreadRt();

//////////////////////////////////////////////////
// Thread Only Functions
//
private:
    /**
     * QueuePop
     */
    [[nodiscard]]
    EQueueMessageType ThreadRt_GetNextMessage();

	/**
	 * Queue Empty
	 */
	[[nodiscard]]
	bool ThreadRt_IsQueueEmpty();

	/**
	 * Block for next Message
	 */
	[[nodiscard]]
	EQueueMessageType ThreadRt_BlockForNextMessage();

	/**
	 * Send Notify over the network
	 */
	void ThreadRt_TransmitToClient(EQueueMessageType Message);

//////////////////////////////////////////////////
// Main Only Functions
//
private:
	/**
	 * Push to Queue
	 */
	void MainRt_PushMessage(EQueueMessageType Message);


//////////////////////////////////////////////////
// Notifiers - Supports
//
public:
    /**
     * Notify of any source of damage boost
     */
    void Notify_DamageBoost();

    /**
     * Notify of incoming healing
     */
    void Notify_Healing();

    /**
     * Notify of the inability to be healed
     */
    void Notify_AntiHealing();


//////////////////////////////////////////////////
// Notifiers - Damage
//
public:
    /**
     * Notify that we hit something
     */
    void Notify_DamageDealt();

    /**
     * Notify that we took damage
     */
    void Notify_DamageTaken();

//////////////////////////////////////////////////
// Notifiers - Health
//
public:
    /**
     * Notify that we hit critical HP
     */
    void Notify_HealthCritical();

    /**
     * Notify that our health changed 
     */
    void Notify_HealthChanged(int newHealth);

};
