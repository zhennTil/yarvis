
#include "server.h"

using namespace std;

void clientHeartbeat(IPaddress &address, AudioAnalyzer *analyzer)
{
	if (analyzer->clientMap.count(address.host) == 0)
	{
		cerr << "WARNING: Heartbeat from unknown host: " << address.host << " : " << address.port << endl;
		return;
	}

	analyzer->clientMap[address.host].last_heartbeat_clock = clock();
}

void clientSubscribe(IPaddress &address, Uint8 &topic, AudioAnalyzer *analyzer)
{
	if (analyzer->clientMap.count(address.host) == 0)
	{
		cout << "INFO: New client added: " << address.host << " : " << address.port << endl;
		analyzer->clientMap[address.host] = ClientDescriptor();
		analyzer->clientMap[address.host].address = address;
		analyzer->clientMap[address.host].last_heartbeat_clock = clock();
		analyzer->clientMap[address.host].subscriptions = set<Uint8>();
	}

	cout << "INFO: Client subscription added: " << (int)topic << endl;
	analyzer->clientMap[address.host].subscriptions.insert(topic);
}
