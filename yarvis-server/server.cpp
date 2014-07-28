
#include "server.h"

#define SDL_HANDLE_ERR() { cerr << "ERROR: " << SDL_GetError() << endl; SDLNet_Quit(); SDL_Quit(); return; }

using namespace std;

//UDPsocket socket;

void initServer(AudioAnalyzer *analyzer)
{
	while (!analyzer->isRunning())
		this_thread::yield();

	std::unordered_map<Uint32, ClientDescriptor> clientMap;

	if (SDL_Init(0) == -1)
		SDL_HANDLE_ERR();

	if (SDLNet_Init() == -1)
		SDL_HANDLE_ERR();

	//socket = SDLNet_UDP_Open(LISTEN_PORT);

	runServerLoops = true;
	listenerThread = thread(&listenerLoop, analyzer);
	senderThread = thread(&senderLoop, analyzer);
}

void stopServer()
{
	runServerLoops = false;
	listenerThread.join();
	senderThread.join();

	//SDLNet_UDP_Close(socket);

	SDLNet_Quit();
	SDL_Quit();
}

void listenerLoop(AudioAnalyzer *analyzer)
{
	UDPsocket socket = SDLNet_UDP_Open(LISTEN_PORT);
	UDPpacket *packet = SDLNet_AllocPacket(INCOMING_PACKET_SIZE);

	while (runServerLoops)
	{
		this_thread::yield();
		
		if (SDLNet_UDP_Recv(socket, packet))
			parsePacket(packet, analyzer);
	}

	SDLNet_FreePacket(packet);
	SDLNet_UDP_Close(socket);
}

void senderLoop(AudioAnalyzer *analyzer)
{
	// Select port for outgoing packages automatically
	UDPsocket socket = SDLNet_UDP_Open(0);
	UDPpacket *packet = SDLNet_AllocPacket(INCOMING_PACKET_SIZE);

	int lastBeat = analyzer->beat->beat_counter;

	while (runServerLoops)
	{
		this_thread::yield();
		// TODO: Send datagrams to subscribers, if appropriate

		// If it's a new beat, send beat notifications to 'beat' subscribers
		if (analyzer->beat->beat_counter != lastBeat)
		{
			lastBeat = analyzer->beat->beat_counter;

			for (auto clientI = analyzer->clientMap.begin(); clientI != analyzer->clientMap.end(); ++clientI)
			{
				if (clientI->second.subscriptions.count(YARP_TOPIC_BEAT) > 0)
				{
					packet->address= clientI->second.address;
					packet->channel = -1;
					
					Uint16 bpm = analyzer->beat->win_bpm_int;
					SDLNet_Write16(bpm, packet->data);
					packet->len = 2;

					cout << "INFO: Sending 'beat' package to " << packet->address.host << ":" << packet->address.port << endl;

					cout << "Sent to " << SDLNet_UDP_Send(socket, -1, packet) << " hosts, " << packet->status << " bytes." << endl;

				}
			}
		}

		// TODO: Prune clients based on heartbeat
	}

	SDLNet_FreePacket(packet);
	SDLNet_UDP_Close(socket);
}

void parsePacket(UDPpacket *packet, AudioAnalyzer *analyzer)
{
	switch (packet->data[0])
	{
	case YARP_TYPE_CLIENT_HEARTBEAT:
		clientHeartbeat(packet->address, analyzer);
		break;
	case YARP_TYPE_CLIENT_SUBSCRIBE:
		clientSubscribe(packet->address, packet->data[1], analyzer);
		break;
	default:
		cerr << "WARNING: Server received unknown packet type '" << packet->data[0] << "'." << endl;
	}
}

