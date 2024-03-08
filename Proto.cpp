#include "proto.hpp"
#include <cstring>

Protocol::Protocol() {
    std::cout << "Protocol constructor" << std::endl;
    if (WSAStartup(MAKEWORD(2, 2), &this->wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        exit(-1);
    }
}

Protocol::~Protocol() {
    WSACleanup();
}

/// Create a new socket fd
Socket Protocol::newSocket() {
    /// Create new socket fd. The options are as follows:

    

   

    // Internet domain      |       Stream socket   |       Default protocol (TCP in this case)
    Socket newSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    return newSock;
}



/// Create a new socket address object using an IP and port
Address Protocol::newSockAddr(IP ipAddr, Port port) {
    Address sockaddrObject;

    /// Configure settings of the server address struct

    // Address family = Internet
    sockaddrObject.sin_family = AF_INET;

    // Set port number, using htons function to use proper byte order
    sockaddrObject.sin_port = htons(port);

    // Set IP address to localhost
    sockaddrObject.sin_addr.s_addr = inet_addr(ipAddr);

    return sockaddrObject;
}

/// Connect a socket to a remote address and return the errno
int Protocol::connectSocket(Socket socket, Address* serverAddr) {
    int connectRes = connect(socket, (const sockaddr*)serverAddr, (socklen_t)sizeof(*serverAddr));
    int derr = errno;

    // There was an error, return errno
    if (connectRes != 0) {
        std::cerr << "Error connecting to " << Protocol::addrToString(*serverAddr) << std::endl;
        Protocol::logErr(derr);
        return derr; // Return errno
    }

    std::cout << "Successfully connected to " << Protocol::addrToString(*serverAddr) << std::endl;


    return connectRes;
}

/// Bind the socket to a port. Attempts <maxAttempts> times
int Protocol::bindSocket(Socket socket, sockaddr_in* serverAddr, unsigned int maxAttempts) {
    int remainingAttempts = maxAttempts;
    int bindResult = -1;
    int derr = -1;

    /// Attempt binding the socket until it works, or until remainingAttempts <= 0
    while ((bindResult = bind(socket, (struct sockaddr*)serverAddr, sizeof(*serverAddr))) && bindResult != 0 && remainingAttempts > 0) {
        derr = errno;

        std::cerr << "There was an error binding to port." << PORT << std::endl;
        Protocol::logErr(derr);

        // Decrement the remaining attempts
        remainingAttempts--;

        // Sleep for 5 seconds, then retry binding
        sleep(5);
        return derr;
    }

    if (bindResult != 0) {
        std::cerr << "Failed binding to port " << PORT << std::endl;
        Protocol::logErr(derr);
        return derr;
    }

    std::cout << "Success! Bound to port " << PORT << std::endl;
    return bindResult;
}

/// Accepts a connection, and returns a socket in which to respond to the new client.
Socket Protocol::acceptConnection(Socket listenSocket, Address* resAddr) {
    socklen_t addr_size = sizeof(*resAddr);
    Socket replySocket = accept(listenSocket, (struct sockaddr*)resAddr, &addr_size);

    std::cout << "Accepted new connection from " << Protocol::addrToString(*resAddr) << std::endl;

    return replySocket;
}

/// Safely shutdown a socket fd and return shutdown's errno
int Protocol::shutdownSocket(Socket socket) {
    int success = shutdown(socket, SHUT_RDWR);
    int derr = errno;

    if (success != 0) {
        std::cerr << "Failed to safely close socket" << std::endl;
        Protocol::logErr(derr);

        return derr;
    }

    return success;
}

std::string Protocol::addrToString(Address addr) {
    std::string joined = inet_ntoa(addr.sin_addr);
    joined += ":";
    joined += std::to_string(ntohs(addr.sin_port));

    return joined;
}

void Protocol::logErr(int _errno) {
    std::cerr << "\033[31m" << "[" << _errno << "]" << strerror(_errno) << "\033[0m" << std::endl;
}