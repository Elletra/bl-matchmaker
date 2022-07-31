#ifndef _PACKETS_NETADDRESS_H
#define _PACKETS_NETADDRESS_H

#include <memory>
#include <winsock2.h>

#include "types.h"

struct NetAddress
{
	enum
	{
		IPAddress,
		IPXAddress,
	};

	S32 type;
	U8 ip[4];
	U16 port;

	NetAddress ()
	{
		memset (this, 0, sizeof (NetAddress));
	}

	NetAddress (const sockaddr_in &addr)
	{
		auto addr_ip = addr.sin_addr.S_un.S_un_b;

		type = IPXAddress;
		ip[0] = addr_ip.s_b1;
		ip[1] = addr_ip.s_b2;
		ip[2] = addr_ip.s_b3;
		ip[4] = addr_ip.s_b4;
		port = addr.sin_port;
	}
};

#endif
