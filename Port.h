#pragma once
#ifndef _PORT_H_
#define _PORT_H_

#define SERVER_IPV4		"192.168.1.1"	//Server's static IPv4 address
#define DESTINATION_TCP_PORT	8080	//A well-known port for TCP socket to listen on the server.
#define DESTINATION_UDP_PORT	56789	//A fixed port for UDP socket for data transfer on the server.
#define SOURCE_UDP_PORT			55555	//A fixed port for UDP socket for data transfer on client.

#endif // !_PORT_H_
