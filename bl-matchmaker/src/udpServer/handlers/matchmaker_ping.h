#ifndef _UDPSERVER_HANDLERS_MATCHMAKER_PING_H
#define _UDPSERVER_HANDLERS_MATCHMAKER_PING_H

#include <vector>

#include "bitStream/BitStream.h"
#include "udpServer/UDPServer.h"
#include "database/DatabaseConnection.h"

#include "NetAddress.h"

#include "types.h"

namespace PacketHandlers
{
	void matchmaker_ping (
		const UDPServer::Packet &packet,
		UDPServer &server,
		DatabaseConnection::Ptr db_conn
	)
	{
		BitStream reader = BitStream (packet.buffer, (S32) packet.num_bytes, (S32) packet.num_bytes);

		U8 type = 0;
		U8 num_addrs = 0;

		reader.read (&type);
		reader.read (&num_addrs);

		std::vector<NetAddress> addresses;

		for (U8 i = 0; i < num_addrs; i++)
		{
			NetAddress addr;

			reader.read (&addr.type);
			reader.read (&addr.ip[0]);
			reader.read (&addr.ip[1]);
			reader.read (&addr.ip[2]);
			reader.read (&addr.ip[3]);
			reader.read (&addr.port);

			addresses.push_back (addr);
		}

		S32 secret = 0;

		reader.read (&secret);

		auto server_addr = packet.address;

		printf ("### Packet type: %s\n", UDPServer::Packet::type_name (type));
		printf ("    Source: %u.%u.%u.%u:%u (%u)\n", server_addr.ip[0], server_addr.ip[1], server_addr.ip[2], server_addr.ip[3], server_addr.port, server_addr.type);
		printf ("    Secret: %d\n", secret);
		printf ("    Number of addresses: %zu\n", addresses.size ());
		printf ("    Addresses:\n");

		for (U32 i = 0; i < addresses.size (); i++)
		{
			NetAddress &addr = addresses[i];

			printf ("    * %u.%u.%u.%u:%u (%u)\n",
				addr.ip[0], addr.ip[1], addr.ip[2], addr.ip[3], addr.port, addr.type);
		}

		if (!db_conn->clear_addresses (server_addr))
		{
			printf ("Error clearing old addresses from database.\n");
			return;
		}

		if (db_conn->add_server (packet.address, addresses))
		{
			printf ("Successfully added server to database!\n");
		}
		else
		{
			printf ("Failed to add server to database!\n");
		}
	}
};

#endif
