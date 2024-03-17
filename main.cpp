#pragma once
#include <vector>
#include <string>
#include "Flight.h"
#include <atomic>
#include <windows.networking.sockets.h>
#include <iostream>
#include <string>
#include "ConfigReader.h"
#include <mutex>
#include <condition_variable>

void activeFlight(Flight* connection);
bool saveData(std::string flightID, double fuelConsumption, time_t timeElapsed, std::string path);
void listeningThread(std::shared_ptr<std::vector<std::pair<std::thread*, Flight*>>> flightRepository, bool* shutdown);
std::mutex lock;
std::condition_variable writeWakeUp;

#pragma comment(lib, "Ws2_32.lib")
void main()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return;

	bool shutdown = false;
	auto flightRepository = std::make_shared<std::vector<std::pair<std::thread*, Flight*>>>();
	std::thread listener(listeningThread, flightRepository, &shutdown);
	//get a connection 

	int opt = 0;

	while(!shutdown)
	{
		std::cout << "Flight server starting... " << std::endl << std::endl;
		//get user input
		std::cout << "Please select an option: " << std::endl;
		std::cout << "1: Shutdown" << std::endl;
		std::cout << std::endl << std::endl;
		opt = std::getchar();
		
		switch (opt)
		{
		case 10:
			shutdown = true;
		default:
			continue;
		}

	}
	if (!flightRepository->empty())
	{
		for (int i = 0; i < flightRepository->size(); i++)
		{
			delete flightRepository->at(i).second;
			flightRepository->at(i).first->join();
			delete flightRepository->at(i).first;
			flightRepository->erase(flightRepository->begin() + i);
		}
	}
	

	listener.join();

}

void listeningThread(std::shared_ptr<std::vector<std::pair<std::thread*, Flight*>>> flightRepository, bool* abort)
{
	ConfigReader configReader("./config.txt");
	std::vector<Config> serverSocketConfigs = configReader.readConfig();
	//there is only one that we need - better than hardcoding a 0
	
	// Create the server we will use to listen for incoming connections
	Server flightListener(serverSocketConfigs.front().address);
	Socket listenerSocket = flightListener.getServerSocket();

	// Loop until the server is told to close
	while (!*abort)
	{

		// Accept new connections in a blocking manner
		std::cout << "Accepting..." << std::endl;
		Connection flightConnection;
		flightConnection.socket = flightListener.accept();
		flightConnection.addr = flightListener.getReplyAddr();

		// Map the connection to the action thread
		std::cout << "Accepted Flight Connection!" << std::endl;
		Flight* flight = new Flight(flightConnection);
		std::thread* connectionThread = new std::thread(activeFlight, flight);
		std::pair<std::thread*, Flight*> newPair = { connectionThread, flight };

		// Push the map to the shared list
		flightRepository->push_back(newPair);

		// CHeck if any of the threads are finished, and destroy them.
		for (int i = 0; i < flightRepository->size(); i++)
		{
			if (flightRepository->at(i).second->getFlightStatus() == false)
			{
				std::cout << "Destroying receive thread" << std::endl;
				delete flightRepository->at(i).second;
				flightRepository->at(i).first->join();
				delete flightRepository->at(i).first;
				flightRepository->erase(flightRepository->begin() + i);
			}
		}
		std::cout << "Listening for more connections..." << std::endl;
	}
}

void activeFlight(Flight* connection)
{
	double fuelSpent = 0;
	double fuelAtLastTransmission = 0;
	double timespan = 0;
	double timeAtLastTransmission = 0;
	double avgConsumption = 0;
	bool flightStatus = true;
	std::string flightID = "";
	Flight* flightConnection = connection;
	

	bool first = true;
	while (flightConnection->getFlightStatus())
	{
		bitstream transmission = flightConnection->getData(first);

		//std::cout << "size is " << transmission.size() << std::endl;

		// Transmission had an error, size will always be 0
		if (transmission.size() == 0) {
			std::cout << "Empty packet" << std::endl;
			flightConnection->disconnect();
			break;
		}

		if (transmission.size() == 1) {
			std::cout << "Error packet" << std::endl;
			flightConnection->disconnect();
			break;
		}

		FlightData* data = flightConnection->deserializeFlightData(transmission, first);
		std::cout << "ID: " << data->flightId << std::endl;
		std::cout << "Time: " << data->timeSinceEpoch << std::endl;

		if (first)
		{
			flightID = data->flightId;
			first = false;
		}
		else
			data->flightId = flightID;

		//write lock
		
		if (data->fuelLevel == 0) {
			std::cout << "Fuel: " << data->fuelLevel << std::endl;
			break;
		}

		//calculation
		flightStatus = flightConnection->getFlightStatus();

		fuelSpent += (fuelAtLastTransmission == 0) ? 0 : data->fuelLevel - fuelAtLastTransmission;
		fuelAtLastTransmission = data->fuelLevel;
		timespan += (timeAtLastTransmission == 0) ? 0 : data->timeSinceEpoch - timeAtLastTransmission;
		timeAtLastTransmission = data->timeSinceEpoch;
		if (timespan != 0)
		{
			avgConsumption = fuelSpent / timespan;
			saveData(data->flightId, avgConsumption, timespan, "./");
		}
	}

	std::cout << "FLight Status is false" << std::endl;
}


bool saveData(std::string flightID, double fuelConsumption, time_t timeElapsed, std::string path)
{
	std::ofstream fStreamout(path + flightID + ".txt", std::ios_base::app | std::ios_base::out);
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