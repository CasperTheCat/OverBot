// Socks
#pragma once
#include "../Header/utils.h"
#include "../../Shared/shared.h"

#ifdef _WIN32
#include <WinSock2.h> // Baremetal Sockets
#include <ws2tcpip.h> // TCPIP Header
#include <queue>
#include <mutex>
#pragma comment(lib, "Ws2_32.lib")

// SocketType
typedef SOCKET crySocket;
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
typedef int crySocket;
#define INVALID_SOCKET -1
#endif

class FrameCommunications
{
private:
#ifdef _WIN32
	std::string WsaPort;
#endif
    std::queue<EQueueMessageType> NetThreadMessageQueue;
    std::mutex NetMutex;
    std::unique_ptr<std::thread> NetThread;
	bool IsBound;
	crySocket ConnectSocket;

protected:
public:

protected:

public:
    FrameCommunications();
    ~FrameCommunications();

    /**
     * Attempt to connect to a host
     */
    [[nodiscard]]
    int ConnectToHost(std::string ip);

	/**
	 * Block for out
	 */
	void Run();

//////////////////////////////////////////////////
// Thread Runtimes
//
private:
	/**
	* Network Thread Runtime
	*/
	void NetworkThreadRt();

	/**
	 * Queue Thread Runtime
	 */
	void QueueThreadRt();

//////////////////////////////////////////////////
// NetThread Only Functions
//
private:
	/**
	* Put an incoming event in queue
	*/
	void NetThreadRt_PostNextMessage(EQueueMessageType Message);

//////////////////////////////////////////////////
// QueueThread Only Functions
//
private:
	/**
	 * ReadQueue, and call any bound event delegates off it
	 */
	void QueueThreadRt_BlockForNextEvent();



protected:
	static void Cleanup();
	static void CloseSocket(crySocket sock);
};