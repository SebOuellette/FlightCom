#include "Server.hpp"

Server::Server(Connection socket)
{
    this->listenSocket = socket;
}

Server::Server(std::string ipAddr) {
    std::cout << "Server has been started" << std::endl;

    /// Create a socket
    this->listenSocket.socket = Protocol::newSocket();

    // Create a new address object for this socket
    this->listenSocket.addr = Protocol::newSockAddr(ipAddr.c_str());

    // Bind the address struct to the socket
    Protocol::bindSocket(this->getServerSocket(), &this->listenSocket.addr);

    /// Listen on the socket, with 5 max connection requests queued
    if (listen(this->getServerSocket(), 5) != 0) {
        int derr = errno;
        std::cerr << "Error listening..." << std::endl;
        Protocol::logErr(derr);
    }
    std::cout << "Listening..." << std::endl;

    memset(&this->replySocket, 0, sizeof(this->replySocket));
}

bitstream Server::send(bitstream& stream) {
    return EndDevice::send(this->getReplySocket(), stream);
}

bitstream Server::recv(int_l size) {
    return EndDevice::recv(this->getReplySocket(), size);
}


Server::~Server() {
    Protocol::shutdownSocket(this->getServerSocket());
    //Protocol::shutdownSocket(this->getReplySocket());
}

// Return the Socket fd for the server's listen socket
Socket Server::getServerSocket() {
    return this->listenSocket.socket;
}

// Return the Address object for the server
Address Server::getServerAddr() {
    return this->listenSocket.addr;
}
// Return the Socket fd for the server's reply socket
Socket Server::getReplySocket() {
    return this->replySocket.socket;
}

// Return the Address object for the client
Address Server::getReplyAddr() {
    return this->replySocket.addr;
}

void Server::setListenSocket(Connection socket)
{
    this->listenSocket = socket;
}


Socket Server::accept() {
    /// Accept call creates a new socket for the incoming connection
    this->replySocket.socket = Protocol::acceptConnection(this->getServerSocket(), &this->replySocket.addr);

    return this->getReplySocket();
}