#include <stdio.h>

#include "UDPServer.h"

UDPServer::UDPServer (const char *hostname, const char *port)
	: hostname (hostname),
	port (port)
{
	memset (&hints, 0, sizeof (hints));
	memset (&client, 0, sizeof (client));
}

UDPServer::~UDPServer ()
{
	stop ();
	state = State::Destroyed;
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
		printf ("Error creating socket: %ld\n", WSAGetLastError ());
		stop ();
		return false;
	}

	result = bind (socket, info->ai_addr, (int) info->ai_addrlen);

	if (result == SOCKET_ERROR)
	{
		printf ("Error binding socket: %ld\n", WSAGetLastError ());
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
	int result = recvfrom (socket, buffer, BUFFER_LEN, 0, (sockaddr *) &client, &client_size);

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
			.buffer_size = BUFFER_LEN,
			.num_bytes = (size_t) result,
		};

		handle_packet (packet);
	}

	return true;
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
		itr->second (*this, packet);
	}
}
