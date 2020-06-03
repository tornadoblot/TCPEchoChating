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
		SocketUtil::ReportError("WSA::StartUp");
		return -1;
	}

	TCPSocketPtr listenSocket = SocketUtil::CreateTCPSocket(INET);
	listenSocket->SetNonBlockingMode(true);
	SocketAddress recevingAddress(INADDR_ANY, 5050);
	if (listenSocket->Bind(recevingAddress) != NO_ERROR)
		SocketUtil::ReportError("Bind");

	std::vector<TCPSocketPtr> readBlockSockets;
	readBlockSockets.push_back(listenSocket);
	std::vector<TCPSocketPtr> readableSockets;
	listenSocket->Listen(5);

	while (true)
	{
		if (!SocketUtil::Select(&readBlockSockets, &readableSockets,
			nullptr, nullptr, nullptr, nullptr))
			continue;

		for (const TCPSocketPtr& socket : readableSockets)
		{
			if (socket == listenSocket)
			{
				SocketAddress newClientAddress;
				auto newSocket = listenSocket->Accept(newClientAddress);
				readBlockSockets.push_back(newSocket);
				std::cout << "New Client\n";
			}
			else
			{
				char segment[GOOD_SEGMENT_SIZE];
				int dataReceived =
					socket->Receive(segment, GOOD_SEGMENT_SIZE);
				if (dataReceived > 0)
				{
					std::cout << "Data Receive: " << segment << '\n';
					socket->Send(segment, sizeof(segment));

					if (!strcmp(segment, "exit"))
						break;
				}
					
			}
		}

	}

	for (TCPSocketPtr& socket : readableSockets)
		socket->~TCPSocket();
	for (TCPSocketPtr& socket : readBlockSockets)
		socket->~TCPSocket();

	WSACleanup();

	return 0;
}
