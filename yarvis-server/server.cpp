
#include "server.h"

#define SDL_HANDLE_ERR() { cerr << "ERROR: " << SDL_GetError() << endl; SDLNet_Quit(); SDL_Quit(); return; }

using namespace std;

void initServer()
{
	if (SDL_Init(0) == -1)
		SDL_HANDLE_ERR();

	if (SDLNet_Init() == -1)
		SDL_HANDLE_ERR();

	runServerLoops = true;
	listenerThread = thread(&listenerLoop);
	senderThread = thread(&senderLoop);
}

void stopServer()
{
	runServerLoops = false;
	listenerThread.join();
	senderThread.join();

	SDLNet_Quit();
	SDL_Quit();
}

void listenerLoop()
{
	UDPsocket socket = SDLNet_UDP_Open(LISTEN_PORT);
	UDPpacket *packet = SDLNet_AllocPacket(INCOMING_PACKET_SIZE);

	while (runServerLoops)
	{
		this_thread::yield();
		
		if (SDLNet_UDP_Recv(socket, packet))
			parsePacket(packet);
	}

	SDLNet_FreePacket(packet);
	SDLNet_UDP_Close(socket);
}

void senderLoop()
{
	while (runServerLoops)
	{
		this_thread::yield();
		// TODO: Send datagrams to subscribers, if appropriate
	}
}

void parsePacket(UDPpacket *packet)
{
	switch (packet->data[0])
	{
	case YARP_TYPE_CLIENT_HEARTBEAT:
		clientHeartbeat(packet->address);
		break;
	case YARP_TYPE_CLIENT_SUBSCRIBE:
		clientSubscribe(packet->address, packet->data[1]);
		break;
	default:
		cerr << "WARNING: Server received unknown packet type '" << packet->data[0] << "'." << endl;
	}
}

