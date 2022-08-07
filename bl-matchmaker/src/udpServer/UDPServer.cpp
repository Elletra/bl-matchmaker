#include <stdio.h>

#include "bitStream/BitStream.h"
#include "config.h"

#include "UDPServer.h"

UDPServer::UDPServer (DatabaseConnection::Ptr db_conn, const char *hostname, const char *port)
	: db_conn (db_conn),
	hostname (hostname),
	port (port)
{
	if (!db_conn->is_open () && !db_conn->open ())
	{
		throw Exception ("UDPServer() - `db_conn` is closed and could not be re-opened");
	}

	memset (&hints, 0, sizeof (hints));
	memset (&client, 0, sizeof (client));
}

UDPServer::~UDPServer ()
{
	stop ();
}

bool UDPServer::start ()
{
	if (state != State::Ready)
	{
		return state == State::Startup || state == State::Running;
	}

	WSADATA wsa_data;

	int result = WSAStartup (MAKEWORD (2, 2), &wsa_data);

	if (result != 0)
	{
		printf ("WSAStartup failed: %d\n", result);
		stop ();
		return false;
	}

	wsa_startup = true;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	hints.ai_flags = AI_PASSIVE;

	result = getaddrinfo (hostname, port, &hints, &info);

	if (result != 0)
	{
		printf ("getaddrinfo failed: %d\n", result);
		stop ();
		return false;
	}

	socket = ::socket (info->ai_family, info->ai_socktype, info->ai_protocol);

	if (socket == INVALID_SOCKET)
	{
		printf ("Error creating socket: %d\n", WSAGetLastError ());
		stop ();
		return false;
	}

	result = bind (socket, info->ai_addr, (int) info->ai_addrlen);

	if (result == SOCKET_ERROR)
	{
		printf ("Error binding socket: %d\n", WSAGetLastError ());
		stop ();
		return false;
	}

	freeaddrinfo (info);
	info = NULL;

	state = State::Running;

	return true;
}

void UDPServer::stop ()
{
	if (state == State::Stopping || state == State::Stopped)
	{
		return;
	}

	state = State::Stopping;

	if (info != NULL)
	{
		freeaddrinfo (info);
		info = NULL;
	}

	if (socket != NULL)
	{
		closesocket (socket);
		socket = NULL;
	}

	if (wsa_startup)
	{
		WSACleanup ();
		wsa_startup = false;
	}

	state = State::Stopped;
}

bool UDPServer::add_handler (Packet::Type type, Packet::Handler handler)
{
	if (!Packet::is_valid_type (type))
	{
		return false;
	}

	if (!has_handler (type, handler))
	{
		handlers.insert (HandlerEntry (type, handler));
	}

	return true;
}

void UDPServer::remove_handler (Packet::Type type, Packet::Handler handler)
{
	std::pair<HandlerMap::iterator, HandlerMap::iterator> itr_pair = handlers.equal_range (type);

	for (HandlerMap::iterator itr = itr_pair.first; itr != itr_pair.second; itr++)
	{
		if (itr->second == handler)
		{
			handlers.erase (itr);
			break;
		}
	}
}

bool UDPServer::has_handler (Packet::Type type, Packet::Handler handler)
{
	std::pair<HandlerMap::iterator, HandlerMap::iterator> itr_pair = handlers.equal_range (type);

	for (HandlerMap::iterator itr = itr_pair.first; itr != itr_pair.second; itr++)
	{
		if (itr->second == handler)
		{
			return true;
		}
	}

	return false;
}

void UDPServer::clear_handlers (Packet::Type type)
{
	handlers.erase (type);
}

void UDPServer::clear_all_handlers ()
{
	handlers.clear ();
}

bool UDPServer::receive ()
{
	int client_size = sizeof (client);
	int result = recvfrom (socket, buffer, MAX_PACKET_DATA_SIZE, 0, (sockaddr *) &client, &client_size);

	if (result == SOCKET_ERROR)
	{
		printf ("recvfrom() error: %d\n", WSAGetLastError ());
		stop ();
		return false;
	}

	if (result > 0 && Packet::is_valid_type (buffer[0]))
	{
		NetAddress address = NetAddress (client);

		Packet packet =
		{
			.type = (Packet::Type) buffer[0],
			.address = address,
			.buffer = buffer,
			.buffer_size = MAX_PACKET_DATA_SIZE,
			.num_bytes = (size_t) result,
		};

		handle_packet (packet);
	}

	return true;
}

bool UDPServer::send (const NetAddress &addr, char *data_buffer, size_t data_size)
{
	if (state != State::Running)
	{
		printf ("Error: Cannot send data packet - server is not running!\n");
		return false;
	}

	if (data_size > MAX_PACKET_DATA_SIZE)
	{
		printf ("Error: Cannot send a data packet larger than %u\n", MAX_PACKET_DATA_SIZE);
		return false;
	}

	addr.to_sockaddr_in (client);

	auto result = sendto (socket, data_buffer, data_size, 0, (sockaddr *) &client, sizeof (client));

	if (result == SOCKET_ERROR)
	{
		printf ("Failed to send data packet: %d\n", WSAGetLastError ());
		return false;
	}

	if (result != data_size)
	{
		printf ("Number of bytes sent does not match data size - possible corruption occurred!\n");
		return false;
	}

	return true;
}

bool UDPServer::send_arranged_request (
	const NetAddress &addr,
	std::vector<NetAddress> &addresses,
	Nonce &nonce_a,
	Nonce &nonce_b
)
{
	char buffer[MAX_PACKET_DATA_SIZE];
	BitStream stream (buffer, MAX_PACKET_DATA_SIZE, MAX_PACKET_DATA_SIZE);

	stream.write ((U8) Packet::Type::ArrangedConnectNotify);
	stream.write (BL_SECRET);

	stream.write (nonce_a.data[0]);
	stream.write (nonce_a.data[1]);

	stream.write (nonce_b.data[0]);
	stream.write (nonce_b.data[1]);

	/* NOTE: This is the `spamConnect` field. I don't know how he calculates this
	   or what its purpose truly is. For now, I'm just having it be false. */
	stream.writeFlag (false);

	stream.write ((U8) addresses.size ());

	for (U32 i = 0; i < addresses.size (); i++)
	{
		stream.write (addresses[i]);
	}

	printf ("Data size: %u\n", stream.getPosition ());

	return send (addr, buffer, stream.getPosition ());
}

void UDPServer::handle_packet (const Packet &packet)
{
	if (packet.buffer_size <= 1 || packet.num_bytes <= 1)
	{
		return;
	}

	std::pair<HandlerMap::iterator, HandlerMap::iterator> itr_pair = handlers.equal_range (packet.type);

	for (HandlerMap::iterator itr = itr_pair.first; itr != itr_pair.second; itr++)
	{
		itr->second (packet, *this, db_conn);
	}
}
