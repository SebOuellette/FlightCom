#pragma once
#include <vector>
#include <string>
#include "Flight.h"
#include <atomic>
#include <windows.networking.sockets.h>
#include <iostream>
#include <string>
#include "ConfigReader.h"

void activeFlight(std::atomic<Flight*> connection);
bool saveData(std::string flightID, double fuelConsumption, time_t timeElapsed, std::string path);
void listeningThread(std::shared_ptr<std::vector<std::pair<std::thread*, Flight*>>> flightRepository, bool* shutdown);

#pragma comment(lib, "Ws2_32.lib")
void main()
{
	bool shutdown = false;
	auto flightRepository = std::make_shared<std::vector<std::pair<std::thread*, Flight*>>>();
	std::thread listener(listeningThread, flightRepository, &shutdown);
	//get a connection 

	int opt = 0;

	while(!shutdown)
	{
		std::cout << "Flight server running. Listening for connections now... " << std::endl << std::endl;
		//get user input
		std::cout << "Please select an option: " << std::endl;
		std::cout << "1: Shutdown";
		opt = std::getchar();
		
		switch (opt)
		{
		case 1:
			shutdown = true;
		default:
			break;
		}

	}
	for (int i = 0; i < flightRepository->size(); i++)
	{
		delete flightRepository->at(i).second;
		flightRepository->at(i).first->join();
		flightRepository->erase(flightRepository->begin() + i);
	}

	listener.join();

}

void listeningThread(std::shared_ptr<std::vector<std::pair<std::thread*, Flight*>>> flightRepository, bool* shutdown)
{
	ConfigReader configReader("/config.txt");
	std::vector<Config> serverSocketConfigs = configReader.readConfig();
	//there is only one that we need - better than hardcoding a 0
	Server flightListener(serverSocketConfigs.front().address);

	while (!shutdown)
	{
		FlightData flightData = FlightData();
		flightListener.accept();
		bitstream serializedConnectionData = flightListener.recv();
		memcpy(&flightData, serializedConnectionData.start(), sizeof(FlightData));
		
		Connection flightConnection;
		flightConnection.addr = flightListener.getServerAddr();
		flightConnection.socket = flightListener.getServerSocket();
		Flight* flight = new Flight(flightConnection);

		std::thread* connectionThread = new std::thread(activeFlight, flight);

		std::pair<std::thread*, Flight*> newPair = { connectionThread, flight };
		flightRepository->push_back(newPair);

		for (int i = 0; i < flightRepository->size(); i++)
		{
			if (flightRepository->at(i).second->getFlightStatus() != true)
			{
				delete flightRepository->at(i).second;
				flightRepository->at(i).first->join();
				flightRepository->erase(flightRepository->begin() + i);
			}
		}
	}
}

void activeFlight(std::atomic<Flight*> connection)
{
	double totalFuel = 0;
	double timespan = 0;
	double timeAtLastTransmission = 0;
	double avgConsumption = 0;
	bool flightStatus = true;
	Flight* flightConnection = nullptr;
	FlightData data;

	while (flightStatus)
	{
		if (!connection.is_lock_free())
			continue;

		flightConnection = connection.load(std::memory_order_relaxed);
		data = flightConnection->getData();

		//calculation
		flightStatus = flightConnection->getFlightStatus();
		connection.store(flightConnection);

		totalFuel += data.fuelLevel;
		timespan += data.timeSinceEpoch - timeAtLastTransmission;
		timeAtLastTransmission = data.timeSinceEpoch;
		if (timespan != 0)
		{
			avgConsumption = totalFuel / timespan;
			saveData(data.flightId, avgConsumption, timespan, "./");
		}
			

	
	}

}


bool saveData(std::string flightID, double fuelConsumption, time_t timeElapsed, std::string path)
{
	std::string filepath = path;

	std::ofstream fStreamout(filepath + flightID + ".txt", std::ios_base::app | std::ios_base::out);
	if (fStreamout.is_open())
	{
		std::string src = flightID + " - " + std::to_string(fuelConsumption)+ "ga/sec" + " elapsed: " + std::to_string(timeElapsed) + "\n";
		fStreamout.write(src.c_str(), src.size());
	}
	else
	{
		return false;
	}
	fStreamout.close();
	return true;

}