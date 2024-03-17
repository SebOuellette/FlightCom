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
            std::cout << "Packet invalid, retrying receive. " << transmittedData.size() << std::endl;
        }

        return transmittedData;
    }

    void disconnect() {
        this->flightData->flightStatus = false;
    }

    bool getFlightStatus()
    {
        return this->flightData->flightStatus;
    }

    // Deserialize a byte stream into FlightData
    FlightData deserializeFlightData(bitstream stream, bool flightIDToggle) {

        BitstreamByte_p ptr = stream.start();

        memcpy(&flightData->Length, ptr, sizeof(int));
        ptr += sizeof(int);

        flightData->flightStatus = (bool)ptr;
        ptr += sizeof(bool);
            
        memcpy(&flightData->fuelLevel, ptr, sizeof(double));
        ptr += sizeof(double);

        flightData->flightId = "999";
        if (flightData->Length > 0)
        {
            
            char* str = new char[flightData->Length + 1]();
            str[flightData->Length] = 0;
            memcpy(str, ptr, flightData->Length);//#define the clientID length 
            ptr += flightData->Length;

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

