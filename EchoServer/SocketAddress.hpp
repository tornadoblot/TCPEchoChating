#pragma once
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdint.h>

class SocketAddress {
public:

	SocketAddress(uint32_t inAddress, uint16_t inPort) 
	{
		GetAsSockAddrIn()->sin_family = AF_INET;
		GetAsSockAddrIn()->sin_addr.S_un.S_addr = htonl(inAddress);
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}

	SocketAddress(const sockaddr& inSockAddr) 
	{
		memcpy(&mSockAddr, &inSockAddr, sizeof(sockaddr));
	}

	SocketAddress(uint32_t inAddress, uint16_t inPort, bool isIPv6)
	{
		GetAsSockAddrIn()->sin_family = AF_INET6;
		GetAsSockAddrIn()->sin_addr.S_un.S_addr = htonl(inAddress);
		GetAsSockAddrIn()->sin_port = htons(inPort);
	}

	SocketAddress()
	{
		GetAsSockAddrIn()->sin_family = AF_INET;
	}

	size_t GetSize() const { return sizeof(sockaddr); }


private:
	friend class UDPSocket;
	friend class TCPSocket;

	sockaddr mSockAddr;
	sockaddr_in* GetAsSockAddrIn()
	{
		return reinterpret_cast<sockaddr_in*>(&mSockAddr);
	}
};

using SocketAddressPtr = std::shared_ptr<SocketAddress>;