#include <stdio.h>
#include <iostream>
#include <memory>
#include <exception>
#include <string>

#include "database/DatabaseConnection.h"

#include "udpServer/UDPServer.h"
#include "udpServer/handlers/matchmaker_ping.h"
#include "udpServer/handlers/arranged_connect_request.h"

#include "types.h"

#include "util/rng.h"
#include "util/string_conversion.h"

#include "config.h"

int main (int argc, const char **argv)
{
	RNG::init ();

	auto db_conn = std::make_shared<DatabaseConnection> ();

	if (!db_conn->open ())
	{
		printf ("Failed to connect to database!\n");
		return 1;
	}

	UDPServer server = UDPServer (db_conn);

	printf ("Starting matchmaker server...\n");

	if (!server.start ())
	{
		printf ("Failed to start matchmaker server!\n");
		return 1;
	}

	server.add_handler (UDPServer::Packet::Type::MatchmakerPing, PacketHandlers::matchmaker_ping);
	server.add_handler (UDPServer::Packet::Type::ArrangedConnectRequest, PacketHandlers::arranged_connect_request);

	printf ("Matchmaker server listening on port %u\n", server.get_port ());

	while (server.is_running () && server.receive ());

	printf ("Matchmaker server shut down.\n");

	return 0;
}
