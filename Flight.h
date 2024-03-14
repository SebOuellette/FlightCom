#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <ctime>

struct FlightData
{
	unsigned int Length;
	bool flightStatus;
	double fuelLevel;
	std::string flightId;
	time_t timeSinceEpoch;

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

// Serialize FlightData into a byte stream
std::string SerializeFlightData(const FlightData& flightData) {
	std::ostringstream oss;

	oss.write(reinterpret_cast<const char*>(&flightData.flightStatus), sizeof(bool));
	oss.write(reinterpret_cast<const char*>(&flightData.Length), sizeof(unsigned int));
	oss.write(reinterpret_cast<const char*>(&flightData.fuelLevel), sizeof(double));

	// Serialize flightId length and data separately
	unsigned int flightIdLength = flightData.flightId.length();
	oss.write(reinterpret_cast<const char*>(&flightIdLength), sizeof(unsigned int));
	oss.write(flightData.flightId.c_str(), flightIdLength);

	oss.write(reinterpret_cast<const char*>(&flightData.timeSinceEpoch), sizeof(time_t));

	return oss.str();
}

// Deserialize a byte stream into FlightData
FlightData DeserializeFlightData(const std::string& data) {
	FlightData flightData;

	std::istringstream iss(data);

	iss.read(reinterpret_cast<char*>(&flightData.flightStatus), sizeof(bool));
	iss.read(reinterpret_cast<char*>(&flightData.Length), sizeof(unsigned int));
	iss.read(reinterpret_cast<char*>(&flightData.fuelLevel), sizeof(double));

	// Deserialize flightId length and data separately
	unsigned int flightIdLength;
	iss.read(reinterpret_cast<char*>(&flightIdLength), sizeof(unsigned int));

	std::vector<char> flightIdBuffer(flightIdLength + 1);
	iss.read(flightIdBuffer.data(), flightIdLength);
	flightIdBuffer[flightIdLength] = '\0'; // Null-terminate the string
	flightData.flightId = flightIdBuffer.data();

	iss.read(reinterpret_cast<char*>(&flightData.timeSinceEpoch), sizeof(time_t));

	return flightData;
}
