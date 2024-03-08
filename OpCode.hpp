#ifndef OPCODE_HPP
#define OPCODE_HPP

#include "bitstream.hpp"


class OpCode {
private:
public:
    virtual bitstream serialize();

};



#endif