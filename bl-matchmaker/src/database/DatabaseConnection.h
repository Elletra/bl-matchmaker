#ifndef _DATABASE_DATABASECONNECTION_H
#define _DATABASE_DATABASECONNECTION_H

#include <time.h>
#include <memory>

#include <pqxx/pqxx>

#include "NetAddress.h"
#include "types.h"

class DatabaseConnection
{
public:
	typedef std::shared_ptr<DatabaseConnection> Ptr;

	enum class State
	{
		Ready,
		Opening,
		Open,
		Closing,
		Closed,
	};

	struct Config
	{
		const char *protocol;
		const char *name;
		const char *username;
		const char *password;
		const char *hostname;
		const char *port;
		const char *params;
	};

private:
	void create_tables ();
	void prepare_statements ();

	pqxx::connection *conn = nullptr;

	bool cleanup_enabled = false;

	time_t cleanup_delay = 10 * 60 * 1000; // 10 minutes
	time_t expire_time = 1 * 60 * 60 * 1000; // 1 hour

	State state = State::Ready;

	Config config;

public:
	DatabaseConnection ();
	DatabaseConnection (Config &config);
	~DatabaseConnection ();

	bool open ();
	void close ();

	bool add_server (const NetAddress &server_addr, const std::vector<NetAddress> addresses);
	bool clear_addresses (const NetAddress &server_addr);
	bool get_addresses (const NetAddress &server_addr, std::vector<NetAddress> &out_addresses);
	bool has_server (const NetAddress &server_addr);

	void cleanup_expired ();

	bool check_matchmaker_token (const NetAddress &addr, const char *token);

	inline State get_state () const { return state; }

	inline bool is_open () const { return state == State::Open; }
	inline bool is_closed () const { return state == State::Closed; }
};

#endif
