#include "TCPSocket.h"
#include "SocketUtil.hpp"
#include "SocketAddressFactory.hpp"
#define GOOD_SEGMENT_SIZE 80

int main()
{
	std::cout << "Server Start..\n";
	WSADATA wsadata;

	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
	{
		SocketUtil::ReportError("WSA::Startup");
		return -1;
	}

	TCPSocketPtr server = SocketUtil::CreateTCPSocket(INET);
	SocketAddress server_addr(INADDR_ANY, 5050);
	if (server->Bind(server_addr) != NOERROR)
		SocketUtil::ReportError("Bind");
	

	server->Listen(5);
	SocketAddress client_addr;
	auto client = server->Accept(client_addr);

	std::cout << "Accept Clear\n";

	while (1)
	{
		char msg[GOOD_SEGMENT_SIZE];
		int dataRecv =
			client->Receive(msg, GOOD_SEGMENT_SIZE);
		if (dataRecv > 0)
		{
			std::cout << "Data Receive: " << msg << '\n'; 
			if (!strcmp(msg, "exit"))
				break;
		}
		client->Send(msg, sizeof(msg));
		memset(msg, 0, sizeof(msg));
	}

	client->~TCPSocket();
	server->~TCPSocket();
	WSACleanup();

	return 0;
}