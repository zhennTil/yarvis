
#pragma once

#include "SDL_net.h"
#include <iostream>
#include <thread>
#include "constants.h"
#include "yarp_constants.h"

static bool runServerLoops = false;
static std::thread listenerThread;
static std::thread senderThread;

void initServer();
void stopServer();

void listenerLoop();
void senderLoop();

void parsePacket(UDPpacket *packet);

// Client package functions
void clientHeartbeat(IPaddress &address);
void clientSubscribe(IPaddress &address, Uint8 &topic);
