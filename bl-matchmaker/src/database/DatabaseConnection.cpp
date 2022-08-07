#include <format>
#include <memory>
#include <string>

#include "config.h"

#include "DatabaseConnection.h"

DatabaseConnection::DatabaseConnection ()
{
	config =
	{
		.protocol = DB_PROTOCOL,
		.name = DB_NAME,
		.username = DB_USER,
		.password = DB_PASSWORD,
		.hostname = DB_HOST,
		.port = DB_PORT,
		.params = DB_PARAMS,
	};
}

DatabaseConnection::DatabaseConnection (Config &config)
	: config (config)
{
}

DatabaseConnection::~DatabaseConnection ()
{
	close ();
}

bool DatabaseConnection::open ()
{
	if (state == State::Open)
	{
		return true;
	}

	printf ("Connecting to database...\n");

	state = State::Opening;

	try
	{
		std::string conn_str = std::format (
			"{}{}:{}@{}:{}/{}{}",
			config.protocol,
			config.username,
			config.password,
			config.hostname,
			config.port,
			config.name,
			strlen (config.params) <= 0 ? "" : "?",
			config.params
		);

		conn = new pqxx::connection (conn_str);

		create_tables ();
		prepare_statements ();
	}
	catch (std::exception &except)
	{
		conn = nullptr;
		printf ("%s\n", except.what ());

		return false;
	}

	state = State::Open;

	printf ("Connected to database.\n");

	return true;
}

void DatabaseConnection::close ()
{
	state = State::Closing;

	if (conn != nullptr)
	{
		conn->close ();
		delete conn;
		conn = nullptr;
	}

	state = State::Closed;
}

bool DatabaseConnection::add_server (const NetAddress &server_addr, const std::vector<NetAddress> addresses)
{
	bool success = true;

	try
	{
		pqxx::work work (*conn);

		auto server_addr_str = server_addr.to_string (true);
		auto result = work.exec_prepared ("add_server", server_addr_str);

		for (U32 i = 0; i < addresses.size (); i++)
		{
			work.exec_prepared ("add_address", addresses[i].to_string (), server_addr_str);
		}

		work.commit ();
	}
	catch (std::exception &except)
	{
		printf ("%s\n", except.what ());
		success = false;
	}

	return success;
}

bool DatabaseConnection::clear_addresses (const NetAddress &server_addr)
{
	bool success = true;

	pqxx::work work (*conn);

	try
	{
		work.exec_prepared ("clear_addresses", server_addr.to_string (true));
		work.commit ();
	}
	catch (std::exception &except)
	{
		printf ("%s\n", except.what ());
		success = false;
	}

	return success;
}

bool DatabaseConnection::get_addresses (const NetAddress &server_addr, std::vector<NetAddress> &out_addresses)
{
	bool success = true;

	out_addresses.clear ();

	try
	{
		pqxx::work work (*conn);

		auto result = work.exec_prepared ("get_addresses", server_addr.to_string (true));

		for (const auto &row : result)
		{
			out_addresses.push_back (NetAddress (row[1].c_str ()));
		}

		success = out_addresses.size () > 0;
	}
	catch (...)
	{
		success = false;
	}

	return success;
}

bool DatabaseConnection::has_server (const NetAddress &server_addr)
{
	bool value = true;

	try
	{
		pqxx::work work (*conn);

		auto result = work.exec_prepared ("has_server", server_addr.to_string (true));
		work.commit ();

		value = result.size () > 0;
	}
	catch (...)
	{
		value = false;
	}

	return value;
}

void DatabaseConnection::cleanup_expired ()
{
	// TODO: Implement occasional expired entry deletion.
}

/// IMPLEMENTME: You would use this function to check if a matchmaker token
///              is the correct one for a certain player.
///
/// @returns Whether the token is valid for this player.
bool DatabaseConnection::check_matchmaker_token (const NetAddress &addr, const char *token)
{
	return true;
}

// ----------------------------------------------------------------------------

/// Creates the tables we need if they don't already exist.
void DatabaseConnection::create_tables ()
{
	if (state != State::Opening || conn == nullptr || !conn->is_open ())
	{
		return;
	}

	pqxx::work work (*conn);

	// FIXME: Remove
	//work.exec ("DROP TABLE servers CASCADE");
	//work.exec ("DROP TABLE addresses");

	// Create table that maintains a list of server addresses that have pinged the matchmaker.
	work.exec ("CREATE TABLE IF NOT EXISTS servers ("
		"addr TEXT PRIMARY KEY UNIQUE NOT NULL, "
		"updated TIMESTAMP WITH TIME ZONE DEFAULT now() NOT NULL "
	")");

	// Create table that maintains a list of available internal IP addresses that a game server has.
	work.exec ("CREATE TABLE IF NOT EXISTS addresses ("
		"id BIGSERIAL PRIMARY KEY UNIQUE NOT NULL, "
		"addr TEXT NOT NULL, "
		"server_addr TEXT REFERENCES servers(addr) NOT NULL, "
		"updated TIMESTAMP WITH TIME ZONE DEFAULT now() NOT NULL, "
		"UNIQUE(addr, server_addr)"
	")");

	work.commit ();
}

void DatabaseConnection::prepare_statements ()
{
	if (state != State::Opening || conn == nullptr || !conn->is_open ())
	{
		return;
	}

	conn->prepare ("add_server",
		"INSERT INTO servers (addr) VALUES ($1) "
		"ON CONFLICT (addr) DO UPDATE SET addr = $1, updated = now() "
		"RETURNING addr"
	);

	conn->prepare ("add_address",
		"INSERT INTO addresses (addr, server_addr) VALUES ($1, $2) "
		"ON CONFLICT (addr, server_addr) DO UPDATE SET updated = now() "
		"RETURNING id"
	);

	conn->prepare ("clear_addresses", "DELETE FROM addresses WHERE server_addr = $1");

	conn->prepare ("get_addresses", "SELECT * FROM addresses WHERE server_addr = $1");
	conn->prepare ("has_server", "SELECT * FROM servers WHERE addr = $1 LIMIT 1");
}
