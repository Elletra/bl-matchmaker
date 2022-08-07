#ifndef _UDPSERVER_UDPSERVER_H
#define _UDPSERVER_UDPSERVER_H

#include <exception>
#include <map>
#include <memory>

#include "database/DatabaseConnection.h"
#include "platform/platform.h"

#include "NetAddress.h"
#include "types.h"

/* This is used by Torque/Blockland. **Do not** change this! */
#define MAX_PACKET_DATA_SIZE 1500

class UDPServer
{
public:
	struct Exception : public std::exception
	{
		Exception (const char *what) : std::exception (what) {}
	};

	enum class State
	{
		Ready,
		Startup,
		Running,
		Stopping,
		Stopped,
	};

	struct Packet
	{
		typedef void (*Handler) (
			const Packet &packet,
			UDPServer &server,
			DatabaseConnection::Ptr db_conn
		);

		// There are more than this, but we only care about these ones.
		enum class Type
		{
			Unknown = -1, // Just a utility member -- it doesn't actually exist in Torque.
			MatchmakerPing = 44,
			ArrangedConnectRequest = 46,
			ArrangedConnectNotify = 48,
		};

		static inline const char *type_name (S32 type)
		{
			switch ((Type) type)
			{
				case Type::MatchmakerPing:
					return "MatchmakerPing";

				case Type::ArrangedConnectRequest:
					return "ArrangedConnectRequest";

				case Type::ArrangedConnectNotify:
					return "ArrangedConnectNotify";

				default:
					return "";
			}
		}

		static inline const char *type_name (Type type) {
			return type_name ((S32) type);
		}

		static inline bool is_valid_type (S32 type) {
			return strcmp (type_name (type), "");
		}
		static inline bool is_valid_type (Type type) {
			return is_valid_type ((S32) type);
		}

		Type type;

		NetAddress &address;

		char *buffer;
		size_t buffer_size;
		size_t num_bytes; // Packets don't usually use the entire buffer, so we need this.
	};

	struct Nonce
	{
		U32 data[2] = { 0, 0 };
	};

private:
	State state = State::Ready;

	struct addrinfo *info = NULL;
	struct addrinfo hints;

	DatabaseConnection::Ptr db_conn;

	const char *hostname;
	const char *port;

	SOCKET socket = NULL;
	sockaddr_in client;

	char buffer[MAX_PACKET_DATA_SIZE];

	bool wsa_startup = false;

	typedef std::multimap<Packet::Type, Packet::Handler> HandlerMap;
	typedef std::pair<Packet::Type, Packet::Handler> HandlerEntry;

	HandlerMap handlers;

public:
	UDPServer (DatabaseConnection::Ptr db_conn, const char *hostname = NULL, const char *port = "5555");
	~UDPServer ();

	inline State get_state () const { return state; }
	inline bool is_running () const { return state == State::Running; }

	inline const char* get_hostname () const { return hostname; };
	inline U16 get_port () const { return atoi (port); };

	bool start ();
	void stop ();

	bool receive ();
	bool send (const NetAddress &addr, char *data_buffer, size_t data_size);

	bool send_arranged_request (
		const NetAddress &addr,
		std::vector<NetAddress> &addresses,
		Nonce &nonce_a,
		Nonce &nonce_b
	);

	void handle_packet (const Packet &packet);

	bool add_handler (Packet::Type type, Packet::Handler handler);
	void remove_handler (Packet::Type type, Packet::Handler handler);
	bool has_handler (Packet::Type type, Packet::Handler handler);
	void clear_handlers (Packet::Type type);
	void clear_all_handlers ();
};

#endif
