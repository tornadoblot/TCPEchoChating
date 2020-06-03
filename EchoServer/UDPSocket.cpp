#include "UDPSocket.hpp"
#include "SocketUtil.hpp"

UDPSocket::~UDPSocket()
{
	closesocket(mSocket);
}

int UDPSocket::Bind(const SocketAddress& inBindAddress)
{
	int err = bind(mSocket,
		&inBindAddress.mSockAddr, inBindAddress.GetSize());

	if(err == 0)
		return NO_ERROR;

	SocketUtil::ReportError("UDPSocket::Bind");
	return -SocketUtil::GetLastError();
}

int UDPSocket::SendTo(
	const void* inData, int inLen, 
	const SocketAddress& inTo)
{
	int byteSentCount = sendto(mSocket, 
		static_cast<const char*>(inData), 
		inLen, 
		0, &inTo.mSockAddr, inTo.GetSize());

	if (byteSentCount >= 0)
		return byteSentCount;

	SocketUtil::ReportError("UDPSocket::SendTo");
	return -SocketUtil::GetLastError();
}

int UDPSocket::ReceiveFrom(
	void* inBuffer, int inMaxLength,
	SocketAddress& outFromAddress)
{
	int fromLength = outFromAddress.GetSize();
	int readByteCount = recvfrom(
		mSocket,
		static_cast<char*>(inBuffer),
		inMaxLength,
		0, &outFromAddress.mSockAddr,
		&fromLength);

	if (readByteCount >= 0)
		return readByteCount;

	SocketUtil::ReportError("UDPSocket::ReceiveFrom");
	return -SocketUtil::GetLastError();
}

int UDPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking)
{
#if _WIN32
	u_long arg = inShouldBeNonBlocking ? 1 : 0;
	int result = ioctlsocket(mSocket, FIONBIO, &arg);

#else
	int flags = fcntl(mSocket, F_GETFL, 0);
	flags = inShouldBeNonBlocking ?
		(flags | O_NONBLOCK) : (flags | ~O_NONBLOCK);
	fcntl(mSocket, F_SETFL, flags);

#endif

	if (result != SOCKET_ERROR)
		return NO_ERROR;

	SocketUtil::ReportError("UDPSocket::SetNonBlockingMode");
	return SocketUtil::GetLastError();

	return 0;
}
