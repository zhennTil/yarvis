
#pragma once

#include "SDL_net.h"
#include <iostream>
#include <thread>
#include "constants.h"

static bool runServerLoops = false;
static std::thread listenerThread;
static std::thread senderThread;

void initServer();
void stopServer();

void listenerLoop();
void senderLoop();

void parsePacket(UDPpacket *packet);
