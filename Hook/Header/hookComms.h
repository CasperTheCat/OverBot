#pragma once

// Socks
#include <WinSock2.h> // Baremetal Sockets
#include <ws2tcpip.h> // TCPIP Header
#pragma comment(lib, "Ws2_32.lib")

enum class EQueueMessageType : uint8_t
{
    NetThreadStart,
    NetThreadStop,
    Signal_EndFrame,

// Supports
    Notify_DamageBoost,
    Notify_Healing,
    Notify_AntiHealing,

    TOTAL_MESSAGE_TYPES // This better not be above 255
};

struct FQueueMessage
{
    EQueueMessageType type;
};

class HookCommunications
{
protected:
	std::string wsaPort;
    std::queue<FQueueMessage> netThreadMessageQueue;
    std::mutex netMutex;
    std::unique_ptr<std::thread> netThread;
	// Server Up?
	bool bIsBound;

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
	bool CreateSocketAndBind();

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
    void NetworkThreadRT();

//////////////////////////////////////////////////
// Thread Only Functions
//
private:
    /**
     * QueuePop
     */
    [[nodiscard]]
    FQueueMessage ThreadRT_GetNextMessage();


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
