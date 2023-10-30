/*
 Copyright (C) 2004 by Matthias Braun <matze@braunis.de>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __SOCKETHEADERS_HPP__
#define __SOCKETHEADERS_HPP__

#ifdef _WIN32
//#//include <winsock2.h>
#include <ws2tcpip.h>

// little fix for mingw
#ifndef AI_NUMERICSERV
 #define AI_NUMERICSERV 0x00000008
#endif

#define SHUTDOWN_BOTH SD_BOTH
#define GET_NET_ERROR() WSAGetLastError()
#define NETSTRERROR(x) "Winsock error: " << x

#define IS_CONNECT_INPROGRESS(code) (code==WSAEWOULDBLOCK)
#define IS_ACCEPT_IGNORABLE(code) ((code==WSAEWOULDBLOCK)||(code==WSAECONNRESET)||(code==WSAEINTR)||(code==WSAEINPROGRESS))
#define IS_DISCONECTED(code) ((code==WSAENETRESET)||(code==WSAECONNABORTED)||(code==WSAETIMEDOUT)||(code==WSAECONNRESET))
#define IS_IGNORABLE_ERROR(code) (code==WSAEWOULDBLOCK)
#define IS_RECVFROM_IGNORABLE(code) ((code==WSAEWOULDBLOCK)||(code==WSAECONNRESET))
#define IS_SENDTO_IGNORABLE(code) ((code==WSAEWOULDBLOCK)||(code==WSAECONNRESET))
#define IS_INVALID_SOCKET(code) (code==WSAENOTSOCK)
#define IS_INTERRUPTED(code) (code==WSAEINTR)
#define SETSOCKOPT_PARAMTYPE char
#define SEND_FLAGS 0
#define RECV_FLAGS 0

#define SETMAXFD(d,o)

#else
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

typedef int SOCKET;
#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define NULL_SOCKET -2
#define closesocket(s) ::close(s)
#define SHUTDOWN_BOTH SHUT_RDWR
#define GET_NET_ERROR() errno
#define NETSTRERROR(x) strerror(x)

#define IS_CONNECT_INPROGRESS(code) (code==EINPROGRESS)
#define IS_ACCEPT_IGNORABLE(code) ((code==EAGAIN)||(code==ECONNABORTED)||(code==EINTR))
#define IS_DISCONECTED(code) ((code==ECONNREFUSED)||(code==ECONNRESET)||(code==EPIPE)||(code==ENOTCONN))
#define IS_IGNORABLE_ERROR(code) ((code==EAGAIN)||(code==EINTR))
#define IS_RECVFROM_IGNORABLE(code) ((code==EAGAIN)||(code==EINTR)||(code==ECONNREFUSED))
#define IS_SENDTO_IGNORABLE(code) ((code==EAGAIN)||(code==EWOULDBLOCK)||(code==EINTR)||(code==EPIPE)||(code==ECONNRESET))
#define IS_INVALID_SOCKET(code) (code==EBADF)
#define IS_INTERRUPTED(code) (code==EINTR)
#define SETSOCKOPT_PARAMTYPE int
#ifdef __APPLE__
#define SEND_FLAGS 0
#define RECV_FLAGS 0
#else
#define SEND_FLAGS MSG_NOSIGNAL
#define RECV_FLAGS MSG_NOSIGNAL
#endif

#define SETMAXFD(d,o) d=(d>o)?d:o

#endif

#endif

