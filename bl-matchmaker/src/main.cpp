#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <conio.h>

#include "udpServer/UDPServer.h"
#include "udpServer/handlers/matchmaker_ping.h"
#include "udpServer/handlers/arranged_connect_request.h"

#include "types.h"

#define BUFFER_LEN 512
#define DATA_SIZE 4

int main (int argc, const char **argv)
{
	UDPServer server = UDPServer ();

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
