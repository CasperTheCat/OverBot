// Socks
#include <WinSock2.h> // Baremetal Sockets
#include <ws2tcpip.h> // TCPIP Header
#include <queue>
#include <mutex>
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

class FrameCommunications
{
private:
	std::string WsaPort;
    std::queue<EQueueMessageType> NetThreadMessageQueue;
    std::mutex netMutex;
    std::unique_ptr<std::thread> NetThread;
	bool bCanReadQueue;

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
    bool ConnectToHost(std::string IP);
};