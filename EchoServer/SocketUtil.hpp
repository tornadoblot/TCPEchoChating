#pragma once
#include "UDPSocket.hpp"
#include "TCPSocket.h"
#include <string>
#include <vector>
#include <Windows.h>;

enum SocketAddressFamily {
	INET = AF_INET,
	INET6 = AF_INET6
};

class SocketUtil
{
public:
	static int GetLastError()
	{
		return WSAGetLastError();
	}
	static void ReportError(std::string error)
	{
		int tmp;
		std::cout << error << " error: " << WSAGetLastError();
		std::cin >> tmp;
		exit(0);
	}
	static UDPSocketPtr CreateUDPSocket(SocketAddressFamily inFamily) 
	{

		SOCKET s = socket(inFamily, SOCK_DGRAM, IPPROTO_UDP);
		if (s != INVALID_SOCKET)
			return UDPSocketPtr(new UDPSocket(s));

		ReportError("SocketUtil::CreateUDPSocket");
		return nullptr;
	}

	static TCPSocketPtr CreateTCPSocket(SocketAddressFamily inFamily)
	{
		SOCKET s = socket(inFamily, SOCK_STREAM, 0);
		if (s != INVALID_SOCKET)
			return TCPSocketPtr(new TCPSocket(s));

		ReportError("SocketUtill::CreateTCPSocket");
		return nullptr;
	}

	static fd_set* FillSetFromVector(
		fd_set& outSet,
		const std::vector<TCPSocketPtr>* inSockets
	) 
	{
		if (inSockets)
		{
			FD_ZERO(&outSet);
			for (const TCPSocketPtr& socket : *inSockets)
			{
				FD_SET(socket->mSocket, &outSet);
			}
			return &outSet;
		}
		else return nullptr;
	}

	static void FillVectorFromSet(
		std::vector<TCPSocketPtr>* outSockets,
		const std::vector<TCPSocketPtr>* inSockets,
		const fd_set& inSet
	)
	{
		if (inSockets && outSockets)
		{
			outSockets->clear();
			for (const TCPSocketPtr& socket : *inSockets)
			{
				if (FD_ISSET(socket->mSocket, &inSet))
					outSockets->push_back(socket);
			}
		}
	}

	static int Select(
		const std::vector<TCPSocketPtr>* inReadSet,
		std::vector<TCPSocketPtr>* outReadSet,
		const std::vector<TCPSocketPtr>* inWriteSet,
		std::vector<TCPSocketPtr>* outWriteSet,
		const std::vector<TCPSocketPtr>* inExceptSet,
		std::vector<TCPSocketPtr>* outExceptSet)
	{
		fd_set read, write, except;
		fd_set* readPtr = FillSetFromVector(read, inReadSet);
		fd_set* writePtr = FillSetFromVector(write, inWriteSet);
		fd_set* exceptPtr = FillSetFromVector(except, inExceptSet);
		int toRet = select(0, readPtr, writePtr, exceptPtr, nullptr);

		if (toRet > 0)
		{
			FillVectorFromSet(outReadSet, inReadSet, read);
			FillVectorFromSet(outWriteSet, inWriteSet, write);
			FillVectorFromSet(outExceptSet, inExceptSet, except);

		}

		return toRet;
	}
};