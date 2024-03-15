#pragma once
#pragma once
#include <string>
#include <map>
#include <thread>
#include "Server.hpp"
#define PACKET_SIZE 24
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
    Flight(Connection connection) : Server(connection) { 
        this->flightData.flightStatus = true;
        this->replySocket = connection;
    }

    FlightData getData(bool &firstPacket)
    {
        int size = firstPacket ? PACKET_SIZE : PACKET_SIZE-10;
        bitstream transmittedData = recv(size);
        firstPacket = false;

        // Check if connection closed
        if (!(transmittedData.size() == PACKET_SIZE || transmittedData.size() == PACKET_SIZE - 10)) {
            this->flightData.flightStatus = false;

            std::cout << "Packet invalid, closing connection whatever " << transmittedData.size() << std::endl;

            FlightData d;
            d.Length = 0;
            return d;
        }

        FlightData flight = deserializeFlightData(transmittedData);
        return flight;
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

