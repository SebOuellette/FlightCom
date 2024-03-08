#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include "Proto.hpp"
#include "EndDevice.hpp"

class Server : private EndDevice {
private:
    Connection listenSocket;
    Connection replySocket;

public:
    Server();
    ~Server();

    bitstream send(bitstream&);
    bitstream recv(int_l = 512);

    Socket accept();

    Socket getServerSocket();
    Address getServerAddr();
    Socket getReplySocket();
    Address getReplyAddr();
};

#endif