#include "Client.hpp"
#include <Windows.h>
#include <iostream>

Client::Client() {
	std::cout << "Started Client..." << std::endl;

	this->server.socket = Protocol::newSocket();

	//this->server.addr = Protocol::newSockAddr("potato.net",//);
}

Client& Client::setConnectionAddr(IP addr, Port port) {
	this->setConnectionAddr(Protocol::newSockAddr(addr));
	this->server.addr.sin_port = htons(port);

	return *this;
}

Client& Client::setConnectionAddr(Address addr) {
	this->server.addr = addr;

	return *this;
}

using namespace std;

Client& Client::connect() {
	// THe case statements should allow to retry
	bool retry = true;
	while (retry) {
		int err = Protocol::connectSocket(this->server.socket, &this->server.addr);
		retry = false;


		switch (err) {
		case 0:
			// Success!
			retry = false;
			break;
		case 106: // Transport endpoint is already connected

			// Close the socket and replace with a new one
			Protocol::shutdownSocket(this->server.socket);
			this->server.socket = Protocol::newSocket();
			std::cout << "Error" << std::endl;
			
			Sleep(1000);

			this->server.addr.sin_port = htons(PORT);
			retry = true;
			break;
		}
	}

	return *this;
}

bitstream Client::send(bitstream& stream) {
	return EndDevice::send(this->getSocket(), stream);
}

bitstream Client::recv(int_l size) {
	return EndDevice::recv(this->getSocket(), size);
}

Socket Client::getSocket() {
	return this->server.socket;
}

Address* Client::getAddress() {
	return &this->server.addr;
}