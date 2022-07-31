#ifndef _UDPSERVER_HANDLERS_ARRANGED_CONNECT_REQUEST_H
#define _UDPSERVER_HANDLERS_ARRANGED_CONNECT_REQUEST_H

#include <vector>

#include "bitStream/BitStream.h"
#include "udpServer/UDPServer.h"

#include "NetAddress.h"

#include "types.h"

namespace PacketHandlers
{
	void arranged_connect_request (const UDPServer &server, const UDPServer::Packet &packet)
	{
		BitStream reader = BitStream (packet.buffer, packet.num_bytes, packet.num_bytes);
		NetAddress addr;

		U8 type = 0;
		U16 request_id = 0;

		reader.read (&type);
		reader.read (addr);
		reader.read (&request_id);

		char token[BUFFER_LEN];
		reader.readString (token);

		printf ("### Packet type: %s\n", UDPServer::Packet::type_name (type));
		printf ("    Address: %u.%u.%u.%u:%u (%u)\n", addr.ip[0], addr.ip[1], addr.ip[2], addr.ip[3], addr.port, addr.type);
		printf ("    Request ID: %u\n", request_id);
		printf ("    Token: %s\n", token);
	}
};

#endif
