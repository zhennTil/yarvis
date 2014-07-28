
#pragma once

#include "SDL_net.h"
#include <set>
#include <ctime>

// Yarp Package Types
#define YARP_TYPE_CLIENT_HEARTBEAT	1
#define YARP_TYPE_CLIENT_SUBSCRIBE	2
#define YARP_TYPE_SERVER_EVENT		128

// Yarp Topic Types
#define YARP_TOPIC_BEAT			1
#define YARP_TOPIC_FFT			2
#define YARP_TOPIC_INTENSITY	3

typedef struct {
	IPaddress address;
	std::set<Uint8> subscriptions;
	clock_t last_heartbeat_clock;
} ClientDescriptor;
