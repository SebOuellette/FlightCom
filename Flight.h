#pragma once
#pragma once
#include <string>
#include <map>
#include <thread>
#include "Server.hpp"

struct FlightData
{
    unsigned char size;
    bool flightStatus;
    double fuelLevel;
    std::string flightId;
    time_t timeSinceEpoch;
};


class Flight : public Server {
private:
    FlightData flightData;
    bitstream serializedflightData;
    
public:
    Flight(Connection connection) : Server(connection) { this->flightData.flightStatus = true; }

    FlightData getData()
    {
        serializedflightData = this->recv();
        memcpy(&flightData.size, serializedflightData.start(), sizeof(FlightData::size));
        memcpy(&flightData.flightStatus, serializedflightData.start() + sizeof(FlightData::size), flightData.size);
        return flightData;
    }

    bool getFlightStatus()
    {
        return flightData.flightStatus;
    }
};

