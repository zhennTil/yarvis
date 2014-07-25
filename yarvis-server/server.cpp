
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
	while (runServerLoops)
	{
		this_thread::yield();
		// TODO: Bind a listener port and parse datagrams
	}
}

void senderLoop()
{
	while (runServerLoops)
	{
		this_thread::yield();
		// TODO: Send datagrams to subscribers, if appropriate
	}
}
