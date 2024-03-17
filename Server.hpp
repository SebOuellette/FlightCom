#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include "Proto.hpp"
#include <atomic>
#include "EndDevice.hpp"

class Server : private EndDevice {
private:
    Connection listenSocket;

protected:
    Connection replySocket;

public:
    Server(Connection);
    Server(std::string);
    ~Server();

    bitstream send(bitstream&);
    bitstream recv(int_l = 512);

    Socket accept();

    Socket getServerSocket();
    Address getServerAddr();
    Socket getReplySocket();
    Address getReplyAddr();

    void setListenSocket(Connection);
};

#endif