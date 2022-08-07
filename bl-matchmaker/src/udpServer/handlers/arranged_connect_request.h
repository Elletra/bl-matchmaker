#ifndef _UDPSERVER_HANDLERS_ARRANGED_CONNECT_REQUEST_H
#define _UDPSERVER_HANDLERS_ARRANGED_CONNECT_REQUEST_H

#include <vector>

#include "bitStream/BitStream.h"
#include "udpServer/UDPServer.h"
#include "database/DatabaseConnection.h"

#include "util/rng.h"

#include "NetAddress.h"
#include "types.h"

#include "config.h"

namespace PacketHandlers
{
	void arranged_connect_request (
		const UDPServer::Packet &packet,
		UDPServer &server,
		DatabaseConnection::Ptr db_conn
	)
	{
		BitStream stream = BitStream (packet.buffer, (S32) packet.num_bytes, (S32) packet.num_bytes);
		NetAddress server_addr;
		U8 type = 0;
		U16 request_id = 0;
		char token[STREAM_STR_BUF_SIZE];

		stream.read (&type);
		stream.read (server_addr);
		stream.read (&request_id);
		stream.readString (token);

		auto source_addr = packet.address;

		printf ("### Packet type: %s\n", UDPServer::Packet::type_name (type));
		printf ("    Source: %u.%u.%u.%u:%u (%u)\n", source_addr.ip[0], source_addr.ip[1], source_addr.ip[2], source_addr.ip[3], source_addr.port, source_addr.type);
		printf ("    Address: %u.%u.%u.%u:%u (%u)\n", server_addr.ip[0], server_addr.ip[1], server_addr.ip[2], server_addr.ip[3], server_addr.port, server_addr.type);
		printf ("    Request ID: %u\n", request_id);
		printf ("    Token: %s\n", token);

		if (!db_conn->check_matchmaker_token (packet.address, token))
		{
			return;
		}

		std::vector<NetAddress> server_addrs;

		if (!db_conn->get_addresses (server_addr, server_addrs))
		{
			return;
		}

		S32 port = 0;
		U8 num_addrs = 0;

		stream.read (&port);
		stream.read (&num_addrs);

		printf ("    Port: %d\n", port);
		printf ("    %u address%s:\n", num_addrs, num_addrs == 1 ? "" : "es");

		std::vector<NetAddress> client_addrs;

		for (U8 i = 0; i < num_addrs; i++)
		{
			NetAddress addr;
			stream.read (addr);

			client_addrs.push_back (addr);

			printf ("        * %s\n", addr.to_string ().c_str ());
		}

		client_addrs.push_back (source_addr);

		UDPServer::Nonce client_nonce;
		UDPServer::Nonce server_nonce;

		client_nonce.data[0] = RNG::generate ();
		client_nonce.data[1] = RNG::generate ();

		/* Believe it or not, this is how Badspot does it... */
		server_nonce.data[0] = client_nonce.data[1];
		server_nonce.data[1] = client_nonce.data[0];

		/* Send list of client addresses to server. */
		server.send_arranged_request (server_addr, client_addrs, server_nonce, client_nonce);

		/* Send list of server addresses to client. */
		server.send_arranged_request (source_addr, server_addrs, client_nonce, server_nonce);
	}
};

#endif
