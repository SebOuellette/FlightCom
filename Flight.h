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
bitstream& serializeFlightData(const FlightData& data) {
	bitstream stream;
	stream << data.Length;
	stream << data.flightStatus;
	stream << data.fuelLevel;
	stream << data.flightId;
	stream << data.timeSinceEpoch;

	return stream;
}

// Deserialize a byte stream into FlightData
FlightData deserializeFlightData(bitstream stream) {
	FlightData flightData;

	memcpy(&flightData, stream.start(), sizeof(FlightData));

	return flightData;
}
