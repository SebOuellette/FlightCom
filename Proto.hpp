#ifndef PROTO_HPP
#define PROTO_HPP

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
//defined to keep the linux variant of GET_STR_ADDR from causing errors for windows/visual studio
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#define SHTDWN_BOTH SD_BOTH
#define SLEEP_TIME(nSeconds) (Sleep(nSeconds) )
#define SET_ADDR_PTR(ipAddr, sockaddrObject) (inet_pton(AF_INET, ipAddr, sockaddrObject))
#define GET_STR_ADDR(sin_addr) (winStrAddr(sin_addr))
#elif __linux__
#include <netinet/in.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define SHTDWN_BOTH SHUT_RDWR
#define SLEEP_TIME(nSeconds) ((sleep(nSeconds) )
#define SET_ADDR_PTR(ipAddr, sockaddrObject) (inet_addr(ipAddr))
#define GET_STR_ADDR(sin_addr) (linStrAddr(sin_addr))
#else
#error "Unrecognized OS"
#endif


#include <cstdint>
#include <string.h>
#include <iostream>
#include "bitstream.hpp"
#define PORT 23512

typedef int Socket;
typedef const char* IP;
typedef std::uint16_t Port;

typedef struct sockaddr_in Address;

typedef struct _connection {
    Address addr;
    Socket socket;
} Connection;

/* Server Process
SOCKET
LISTEN
BIND
ACCEPT
SEND/RECV
*/

/* Client Process
SOCKET
CONNECT
SEND/RECV
*/

// Data in this protocol is in little endian format
class Protocol {
private:
    int author;
    char* message;
    WSADATA wsaData;

public:
    Protocol();
    ~Protocol();

    static Socket newSocket();
    static Address newSockAddr(IP ipAddr, Port port = PORT);
    static int connectSocket(Socket socket, Address* serverAddr);
    static int bindSocket(Socket socket, Address* serverAddr, unsigned int maxAttempts = 5);
    static Socket acceptConnection(Socket listenSocket, Address* resAddr);
    static int shutdownSocket(Socket socket);

    static std::string addrToString(Address addr);

    static void logErr(int _errno);
};

class SocketWrapper {
private:
    Connection socket;

public:

};

#endif