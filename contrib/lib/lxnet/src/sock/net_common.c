
/*
 * Copyright (C) lcinx
 * lcinx@163.com
*/

#include "net_common.h"

#ifdef WIN32

#ifdef _MSC_VER
#pragma comment(lib,"Ws2_32")
#endif

#else

#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>

#endif

net_socket socket_create ()
{
	return socket(AF_INET, SOCK_STREAM, 0);
}

int socket_close (net_socket *sockfd)
{
	int res;
#ifdef WIN32
	res = closesocket(*sockfd);
#else
	res = close(*sockfd);
#endif
	*sockfd = NET_INVALID_SOCKET;
	return res;
}

static bool socket_set_nonblock (net_socket sockfd)
{
#ifdef WIN32
	{
		unsigned long nonblocking = 1;
		ioctlsocket(sockfd, FIONBIO, (unsigned long*) &nonblocking);
	}
#else
	{
		long flags;
		if ((flags = fcntl(sockfd, F_GETFL, NULL)) < 0)
			return false;

		if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
			return false;
	}
#endif
	return true;
}

bool socket_setopt_for_connect (net_socket sockfd)
{
	if (!socket_set_nonblock(sockfd))
		return false;

	/* prohibit nagle. */
	{
		int bNoDelay = 1;
		setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, (char*)&bNoDelay, sizeof(int));
	}

	{
		struct linger ling;
		ling.l_onoff=1;
		ling.l_linger=0;
		setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (char *)&ling, sizeof(ling));
	}
	/*int bDontLinger = true;

	setsockopt(sockfd, SOL_SOCKET, SO_DONTLINGER, (char *)&bDontLinger, sizeof(int) );
	int iSize = sizeof(int);
	int iRet = getsockopt(sockfd,SOL_SOCKET,SO_DONTLINGER,(char *)&bDontLinger, &iSize);*/

	{
		int bKeepAlive = 1;
		setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char*)&bKeepAlive, sizeof(int));
	}

	return true;
}

bool socket_setopt_for_listen (net_socket sockfd)
{
	return socket_set_nonblock(sockfd);
}
