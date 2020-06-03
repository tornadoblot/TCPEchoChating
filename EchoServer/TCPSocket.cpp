#include "TCPSocket.h"
#include "SocketUtil.hpp"
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

TCPSocket::~TCPSocket()
{
	closesocket(mSocket);
}

int TCPSocket::Connect(const SocketAddress& inAddress)
{
	int err = connect(mSocket, &inAddress.mSockAddr, inAddress.GetSize());

	if (err >= 0)
		return NO_ERROR;

	SocketUtil::ReportError("TCPSocket::Connect");
	return -SocketUtil::GetLastError();
}

int TCPSocket::Bind(const SocketAddress& inToAddress)
{
	int err = bind(mSocket,
		&inToAddress.mSockAddr, inToAddress.GetSize());

	if (err == 0)
		return NO_ERROR;

	SocketUtil::ReportError("TCPSocket::Bind");
	return -SocketUtil::GetLastError();
}

int TCPSocket::Listen(int inBackLog)
{
	int err = listen(mSocket, inBackLog);

	if (err >= 0)
		return NO_ERROR;

	SocketUtil::ReportError("TCPSocket::Listen");
	return -SocketUtil::GetLastError();
}

TCPSocketPtr TCPSocket::Accept(SocketAddress& inFromAddress)
{
	int length = inFromAddress.GetSize();
	SOCKET newSocket = accept(mSocket, &inFromAddress.mSockAddr, &length);

	if (newSocket != INVALID_SOCKET)
		return TCPSocketPtr(new TCPSocket(newSocket));

	SocketUtil::ReportError("TCPSocket::Accept");
	return nullptr;
}

int TCPSocket::Send(const void* inData, int inLen)
{
	int bytesSentCount = send(
		mSocket,
		static_cast<const char*>(inData),
		inLen, 0);

	if (bytesSentCount >= 0)
		return bytesSentCount;

	SocketUtil::ReportError("TCPSocket::Send");
	return -SocketUtil::GetLastError();
}

int TCPSocket::Receive(void* inData, int inLen)
{
	int bytesReceivedCount = recv(
		mSocket,
		static_cast<char*>(inData),
		inLen, 0);

	if (bytesReceivedCount >= 0)
		return bytesReceivedCount;

	SocketUtil::ReportError("TCPSocket::Receive");
	return -SocketUtil::GetLastError();
}

int TCPSocket::SetNonBlockingMode(bool isShouldBeNonBlocking)
{
#if _WIN32
	u_long arg = isShouldBeNonBlocking ? 1 : 0;
	int result = ioctlsocket(mSocket, FIONBIO, &arg);
#else

	int flags = fcntl(mSocket, F_GETFL, 0);
	flags = isShouldBeNonBlocking ?
		(flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
	fcntl(mSocket, F_SETFL, flags);

#endif

	if (result != SOCKET_ERROR)
		return NO_ERROR;

	SocketUtil::ReportError("TCPSocket::SetNonBlocingMode");
	return SocketUtil::GetLastError();
	return 0;
}