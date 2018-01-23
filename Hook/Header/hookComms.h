#pragma once

// Socks
#include <WinSock2.h> // Baremetal Sockets
#include <ws2tcpip.h> // TCPIP Header
#pragma comment(lib, "Ws2_32.lib")


class HookCommunications
{
protected:
	std::string wsaPort;

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
     * Send Notify to the publisher
     */
    void Notify_DamageBoost();
};
