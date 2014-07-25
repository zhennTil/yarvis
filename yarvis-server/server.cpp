
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
	// TODO: Parse packet
}

