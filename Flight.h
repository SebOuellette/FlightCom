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
    FlightData* flightData;
    bitstream serializedflightData;
    
public:
    Flight(Connection connection) : Server(connection) { 

        flightData = new FlightData();
        flightData->flightStatus = true;
        this->replySocket = connection;
    }

    bitstream getData(bool &firstPacket)
    {
        int size = firstPacket ? PACKET_SIZE : PACKET_SIZE-10;
        bitstream transmittedData = recv(size);

        // Check if connection closed
        if (!(transmittedData.size() == PACKET_SIZE || transmittedData.size() == PACKET_SIZE - 10)) {
            flightData->flightStatus = false;

            std::cout << "Packet invalid, closing connection whatever " << transmittedData.size() << std::endl;

            bitstream d;
            return d;
        }
        if (transmittedData.size() == 0)
        {
            //something is wrong
            int something = 0;
        }
        return transmittedData;
    }

    bool getFlightStatus()
    {
        bool status = flightData->flightStatus;
        return status;
    }

    // Deserialize a byte stream into FlightData
    FlightData deserializeFlightData(bitstream stream, bool flightIDToggle) {

        BitstreamByte_p ptr = stream.start();

        memcpy(&flightData->Length, ptr, sizeof(int));
        ptr += sizeof(int);

        memcpy(&flightData->flightStatus, ptr, sizeof(bool));
        ptr += sizeof(bool);

        memcpy(&flightData->fuelLevel, ptr, sizeof(double));
        ptr += sizeof(double);

        if (flightIDToggle)
        {
            flightData->flightId = "999";
            char* str = new char[10 + 1]();
            memcpy(str, ptr, 10);//#define the clientID length 
            ptr += 10;

            // Copy char array to string
            flightData->flightId = str;
            delete[] str;
        }

        memcpy(&flightData->timeSinceEpoch, ptr, sizeof(int));
        ptr += sizeof(int);

        return *flightData;
    }

    ~Flight()
    {
        if(flightData != nullptr)
            delete flightData;
    }
};

