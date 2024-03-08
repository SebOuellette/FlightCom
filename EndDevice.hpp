#ifndef ENDDEVICE_HPP
#define ENDDEVICE_HPP

#include <iostream>
#include <vector>
#include "proto.hpp"

#define RECV_ERR(n) n < 0


class EndDevice {
private:
public:
    EndDevice();

    static bitstream send(int socket, bitstream&);
    static bitstream recv(int socket, int_l = 512);

};

#endif