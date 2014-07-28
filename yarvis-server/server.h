
#pragma once

#include "SDL_net.h"
#include <iostream>
#include <unordered_map>
#include <set>
#include <thread>
#include "constants.h"
#include "yarp_constants.h"
#include "audio_analyzer.h"

static bool runServerLoops = false;
static std::thread listenerThread;
static std::thread senderThread;

void initServer(AudioAnalyzer *analyzer);
void stopServer();

void listenerLoop(AudioAnalyzer *analyzer);
void senderLoop(AudioAnalyzer *analyzer);

void parsePacket(UDPpacket *packet, AudioAnalyzer *analyzer);

// Client package functions
void clientHeartbeat(IPaddress &address, AudioAnalyzer *analyzer);
void clientSubscribe(IPaddress &address, Uint8 &topic, AudioAnalyzer *analyzer);

