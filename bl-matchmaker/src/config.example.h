#ifndef _CONFIG_H
#define _CONFIG_H

/// This is an example configuration for server, including database credentials,
/// server port/address, etc.
///
/// Feel free to change these with your own values.
///
/// **NOTE** You must copy this file to one named `config.h` in order for this
///          program to build!
///
///          This is to prevent sensitive information and custom configurations
///          from being committed to the repo.

/**
 * Config for UDP server
 */

#define SERVER_HOST "localhost"
#define SERVER_PORT "5555"

/**
 * Config for PostgreSQL database
 */

/* Database protocol */
#define DB_PROTOCOL "postgres://"

/* Database name */
#define DB_NAME "blmatchmaker"

/* Database user credentials */
#define DB_USER "blmatchmaker"
#define DB_PASSWORD "password"
#define DB_USER_PASS DB_USER ":" DB_PASSWORD

/* Database hostname and port */
#define DB_HOST "localhost"
#define DB_PORT "5432"
#define DB_HOST_PORT DB_HOST ":" DB_PORT

/* Database parameters, if any */
#define DB_PARAMS ""

/* The connection string used to create a connection to the database */
#define DB_CONN_STR (DB_PROTOCOL DB_USER_PASS "@" DB_HOST_PORT "/" DB_NAME DB_PARAMS)

#endif
