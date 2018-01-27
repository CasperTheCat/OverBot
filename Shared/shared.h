#pragma once
#include <cstdint>

enum class EQueueMessageType : uint8_t
{
	NullSignal = 0,
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