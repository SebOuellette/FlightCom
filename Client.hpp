#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iostream>
#include "Proto.hpp"
#include "EndDevice.hpp"
#include <Windows.h>

class Client : private EndDevice {
private:
	Connection server;

public:
	Client();

	Client& setConnectionAddr(IP addr, Port port = PORT);
	Client& setConnectionAddr(Address addr);
	Client& connect();

	bitstream send(bitstream&);
	bitstream recv(int_l = 512);

	Socket getSocket();
	Address* getAddress();
};

#endif