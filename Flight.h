#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <ctime>
#include "bitstream.hpp"

struct FlightData
{
	int Length;
	bool flightStatus;
	double fuelLevel;
	std::string flightId;
	int timeSinceEpoch;

public:
	FlightData()
	{
		flightStatus = true;
		fuelLevel = 0.0;
		flightId = "";
		timeSinceEpoch = std::time(nullptr);
	}

	void setFlightStatus(bool flightStatus)
	{
		this->flightStatus = flightStatus;
	}

	void setFuelLevel(double fuelLevel)
	{
		this->fuelLevel = fuelLevel;
	}

	void setFlightId(std::string flightId)
	{
		this->flightId = flightId;
	}

	void setTimeSinceEpoch(time_t timeSinceEpoch)
	{
		this->timeSinceEpoch = timeSinceEpoch;
	}

	bool getFlightStatus()
	{
		return flightStatus;
	}

	double getFuelLevel()
	{
		return fuelLevel;
	}

	std::string getFlightId()
	{
		return flightId;
	}

	time_t getTimeSinceEpoch()
	{
		return timeSinceEpoch;
	}
};

// Function to serialize FlightData
bitstream serializeFlightData(const FlightData& data) {
	bitstream stream;
	stream << (char)(data.flightId.length() & 0xFF);
	stream << data.flightStatus;
	stream << data.fuelLevel;
	stream << data.flightId;
	stream << data.timeSinceEpoch;

	return stream;
}

// Deserialize a byte stream into FlightData
FlightData deserializeFlightData(bitstream stream) {
	FlightData flightData;

	BitstreamByte_p ptr = stream.start();
	
	char x;
	memcpy(&x, ptr, sizeof(char));
	ptr += sizeof(char);
	flightData.Length = x;

	memcpy(&flightData.flightStatus, ptr, sizeof(bool));
	ptr += sizeof(bool);

	memcpy(&flightData.fuelLevel, ptr, sizeof(double));
	ptr += sizeof(double);

	// Calculate size of flightData
	int strLen = flightData.Length; // Length of string
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
