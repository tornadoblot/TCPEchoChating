#include "TCPSocket.h"
#include "SocketUtil.hpp"
#include "SocketAddressFactory.hpp"
#define GOOD_SEGMENT_SIZE 80

bool isEnd = false;

int main()
{
	WSAData wsaData;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		SocketUtil::ReportError("Main::MakeWord");

	TCPSocketPtr socket = SocketUtil::CreateTCPSocket(INET);
	SocketAddressPtr sendingAddress
		= SocketAddressFactory::CreateIPv4FromString("127.0.0.1:48000");

	socket->Connect(*sendingAddress);

	std::cout << "���� ����\n";
	while (1)
	{
		char msg[GOOD_SEGMENT_SIZE];
		int dataSend;
		std::cout << "�޼����� �Է����ּ���: ";
		std::cin >> msg;

		dataSend = socket->Send(msg, sizeof(msg));

		if (!strcmp(msg, "exit"))
			break;

		char segment[GOOD_SEGMENT_SIZE];

		int dataReceived =
			socket->Receive(segment, GOOD_SEGMENT_SIZE);

		if (dataReceived > 0)
		{
			std::cout << "����: " << segment << '\n';
		}
	}

	socket->~TCPSocket();
	WSACleanup();
}