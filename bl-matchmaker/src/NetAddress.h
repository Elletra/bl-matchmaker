#ifndef _PACKETS_NETADDRESS_H
#define _PACKETS_NETADDRESS_H

#include <memory>
#include <string>

#include <pqxx/pqxx>

#include "types.h"

#include "platform/platform.h"
#include "util/string_conversion.h"

struct NetAddress
{
	enum
	{
		IPXAddress, // Not used by the matchmaker stuff, as far as I can tell...
		IPAddress,
	};

	enum Default
	{
		Type = IPAddress,
		Port = 28000,
	};

	S32 type;
	U8 ip[4];
	U16 port;

	NetAddress ()
	{
		clear ();
	}

	NetAddress (const char *str)
	{
		from_string (str);
	}

	NetAddress (U8 ip0, U8 ip1, U8 ip2, U8 ip3, U16 port = Default::Port, S32 type = Default::Type)
		: port (port),
		type (type)
	{
		ip[0] = ip0;
		ip[1] = ip1;
		ip[2] = ip2;
		ip[3] = ip3;
	}

	NetAddress (const sockaddr_in &addr)
	{
		auto addr_ip = addr.sin_addr.S_un.S_un_b;

		type = Default::Type;
		ip[0] = addr_ip.s_b1;
		ip[1] = addr_ip.s_b2;
		ip[2] = addr_ip.s_b3;
		ip[3] = addr_ip.s_b4;
		port = addr.sin_port;
	}

	inline void clear ()
	{
		type = Default::Type;
		ip[0] = 0;
		ip[1] = 0;
		ip[2] = 0;
		ip[3] = 0;
		port = Default::Port;
	}

	std::string to_string (bool ip_only = false) const
	{
		if (ip_only)
		{
			return std::format ("{}.{}.{}.{}", ip[0], ip[1], ip[2], ip[3]);
		}

		return std::format ("{}.{}.{}.{}:{}={}", ip[0], ip[1], ip[2], ip[3], port, type);
	}

	void from_string (const char *str)
	{
		clear ();

		size_t index = 0;
		std::string string (str);

		index = str_to_u8 (string, index, ip[0]);
		index = str_to_u8 (string, index + 1, ip[1]);
		index = str_to_u8 (string, index + 1, ip[2]);
		index = str_to_u8 (string, index + 1, ip[3]);
		index = str_to_u16 (string, index + 1, port);
		str_to_s32 (string, index + 1, type);
	}

	void to_sockaddr_in (sockaddr_in &addr) const
	{
		auto addr_ip = addr.sin_addr.S_un.S_un_b;

		addr_ip.s_b1 = ip[0];
		addr_ip.s_b2 = ip[1];
		addr_ip.s_b3 = ip[2];
		addr_ip.s_b4 = ip[3];
		addr.sin_port = port;
		addr.sin_family = AF_INET;
	}
};

#endif
