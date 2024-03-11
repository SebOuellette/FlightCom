#include <vector>
#include <string>
#include "Flight.h"
#include <atomic>
#include <windows.networking.sockets.h>
#include <iostream>
#include <string>

void listeningThread(std::shared_ptr<std::vector<std::pair<std::thread, Flight*>>> flightRepository, bool* shutdown);

#pragma comment(lib, "Ws2_32.lib")
void main()
{
	bool shutdown = false;
	std::shared_ptr<std::vector<std::pair<std::thread, Flight*>>> flightRepository = std::make_shared < std::vector<std::pair<std::thread, Flight*>>>();
	std::thread listener(listeningThread, flightRepository, &shutdown);
	//get a connection 

	while(!shutdown)
	{//do nothing



	}

	listener.join();

}

void listeningThread(std::shared_ptr<std::vector<std::pair<std::thread, Flight*>>> flightRepository, bool* shutdown)
{
	Server flightListener("127.0.0.1");

	while (!shutdown)
	{
		FlightData flightData = FlightData();
		flightListener.accept();
		bitstream serializedConnectionData = flightListener.recv();
		memcpy(&flightData, serializedConnectionData.start(), sizeof(FlightData));
		
		Socket flightSocket = flightListener.getReplySocket();
		Address flightAddress = flightListener.getReplyAddr();
		Connection flightConnection;
		flightConnection.addr = flightAddress;
		flightConnection.socket = flightSocket;
		Flight* flight = new Flight(flightConnection);

		std::thread connectionThread(activeFlight, flightConnection);

		flightRepository->push_back(std::make_pair(connectionThread, flight));

		for (int i = 0; i < flightRepository->size(); i++)
		{
			if (flightRepository->at(i).second->getFlightStatus() != true)
			{
				flightRepository->at(i).first.join();
				delete flightRepository->at(i).second;
			}
		}
	}

	for (int i = 0; i < flightRepository->size(); i++)
	{
		flightRepository->at(i).first.join();
		delete flightRepository->at(i).second;
	}

}

void activeFlight(std::atomic<Flight*> connection)
{
	double totalFuel = 0;
	double timespan = 0;
	double timeAtLastTransmission = 0;
	double avgConsumption = 0;
	bool flightStatus = true;
	Flight* flightConnection = connection.load(std::memory_order_relaxed);
	FlightData data;

	while (flightStatus)
	{
		data = flightConnection->getData();

		//calculation
		flightStatus = flightConnection->getFlightStatus();
		totalFuel += data.fuelLevel;
		timespan += data.timeSinceEpoch - timeAtLastTransmission;
		timeAtLastTransmission = data.timeSinceEpoch;
		if(timespan != 0)
			avgConsumption = totalFuel / timespan;
	}

}