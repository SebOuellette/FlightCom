
#define _CRT_SECURE_NO_WARNINGS


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
#include <iomanip>
#include <sstream>


std::string TimeToString();
bool saveTime(std::vector<std::string> times);
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
	std::vector<std::string> arrivalTimes;
	
	bool first = true;
	while (flightConnection->getFlightStatus())
	{
		bitstream transmission = flightConnection->getData(first);
		//std::string currentTime = TimeToString();
		//arrivalTimes.push_back(currentTime);
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
		
		if (data->fuelLevel == 0) {
			std::cout << "Fuel: " << data->fuelLevel << std::endl;
			break;
		}

		//calculation
		flightStatus = flightConnection->getFlightStatus();

		fuelSpent += (fuelAtLastTransmission == 0) ? 0 : fuelAtLastTransmission - data->fuelLevel;
		fuelAtLastTransmission = data->fuelLevel;
		timespan += (timeAtLastTransmission == 0) ? 0 : data->timeSinceEpoch - timeAtLastTransmission;
		timeAtLastTransmission = data->timeSinceEpoch;
		if (timespan != 0)
		{
			avgConsumption = fuelSpent / timespan;
			saveData(data->flightId, avgConsumption, timespan, "./");
		}

		transmission.clear();
	}

	std::cout << "FLight Status is false" << std::endl;
	//saveTime(arrivalTimes);
}

bool saveTime(std::vector<std::string> times)
{

	std::ofstream fStreamout("arrivalTimes.txt", std::ios_base::app | std::ios_base::out);
	if (fStreamout.is_open())
	{
		for(std::string& timeStr : times)
		{
			fStreamout.write(timeStr.c_str(), timeStr.size());
		}
		
	}
	else
	{
		return false;
	}
	fStreamout.close();
	return true;
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

std::string TimeToString()
{
	time_t now = time(0);
	tm* timeStruct = localtime(&now);

	// Parse the time string into the tm structure
	std::string newTime;
	std::istringstream ss(newTime);
	ss >> std::get_time(timeStruct, "%H:%M:%S");

	// Check for successful parsing
	if (ss.fail()) {
		std::cerr << "Failed to parse the time string." << std::endl;
	}

	return ss.str() + "\n";

}