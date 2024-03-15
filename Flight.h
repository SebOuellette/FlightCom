#pragma once
#pragma once
#include <string>
#include <map>
#include <thread>
#include "Server.hpp"

struct FlightData
{
    unsigned char Length;
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
        bitstream transmittedData = recv();
        deserializeFlightData(transmittedData);
        return flightData;
    }

    bool getFlightStatus()
    {
        return flightData.flightStatus;
    }

    // Deserialize a byte stream into FlightData
    FlightData deserializeFlightData(bitstream stream) {
        FlightData flightData;

        BitstreamByte_p ptr = stream.start();

        memcpy(&flightData.Length, ptr, sizeof(int));
        ptr += sizeof(int);

        memcpy(&flightData.flightStatus, ptr, sizeof(bool));
        ptr += sizeof(bool);

        memcpy(&flightData.fuelLevel, ptr, sizeof(double));
        ptr += sizeof(double);

        // Calculate size of flightData
        int strLen = 10;// flightData.Length - 2 * sizeof(int) - sizeof(bool) - sizeof(double) - 10;
        std::cout << "strlen is " << strLen << std::endl;
        char* str = new char[strLen + 1]();

        // Copy data into char array
        memcpy(str, ptr, strLen);
        // Copy char array to string
        flightData.flightId = str;

        delete[] str;
        ptr += strLen;

        memcpy(&flightData.timeSinceEpoch, ptr, sizeof(int));
        ptr += sizeof(int);


        return flightData;
    }
};

