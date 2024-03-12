#pragma once
#include <string>
struct FlightData
{
	bool flightStatus;
	double fuelLevel;
	std::string flightId;
	time_t timeSinceEpoch;
};
