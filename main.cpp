#include <vector>
#include <string>
#include "Flight.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <iomanip>
#include "GenerateID.h"
#include "Client.hpp"
#include "thread"

time_t stringToTime(std::string line);
void ParseLine(std::string line, double& currentFuel, time_t& time);
void ClientConnection(std::vector<FlightData*>* dataStreams);
void SpawnClient(bool* stop);

#pragma comment(lib, "Ws2_32.lib")

void main() {
	srand(time(0));
	//get input from user - number of clients to spawn
	int numberOfClients = 1;
	bool stop = false;
	std::vector<std::thread*> clientThreads;

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return;

	for (int i = 0; i < numberOfClients; i++)
	{
		std::thread* clientThread = new std::thread(SpawnClient, &stop);
		clientThreads.push_back(clientThread);
	}
	//dont stop until it is terminated automatically
	while (!stop) {};
	for (std::thread*& client : clientThreads)
	{
		client->join();
		delete client;
	}
	//End of Program
}

void SpawnClient(bool* stop)
{
	// Open connection / Connect to server
	//std::thread* sendingThread = nullptr;
	//std::vector<FlightData*>* transmissions = new std::vector<FlightData*>();
	Client c;
	//c.setConnectionAddr("10.144.104.228", 23512).connect();
	c.setConnectionAddr("127.0.0.1", 23512).connect();


	std::cout << "Connected to server" << std::endl;

	FlightData* flightData = nullptr;
	bool firstPacket = true;

	//generate flightId
	GenerateID generateID = GenerateID();
	std::string flightID = generateID.GetRandomSequence(10);

	// initialize Possible flight records
	std::vector<std::string> flightRecords{
		"Telem_2023_3_12 16_26_4.txt",
		"Telem_czba-cykf-pa28-w2_2023_3_1 12_31_27.txt",
		"katl-kefd-B737-700.txt",
		"Telem_2023_3_12 14_56_40.txt"
	};

	// Define the range
	int minRange = 0;
	int maxRange = 3;

	//randomize record selection (doesn't have to be true randomization)
	//
	int randomNumber = minRange + (std::rand() + flightID[0]) % (maxRange - minRange + 1);

	// Open selected randomized file
	std::ifstream readFile(flightRecords[randomNumber]);
	std::cout << "file opened: " << flightRecords[randomNumber] << std::endl;

	std::string line = "";
	time_t time;
	double currentFuel = 0;

	// Get the first line which we dont need for the circumstance and ignore it
	std::getline(readFile, line);

	//sendingThread = new std::thread(ClientConnection, transmissions);

	// loop and read file until the EOF
	unsigned long long int timeLast = 0;
	while (std::getline(readFile, line))
	{
		ParseLine(line, currentFuel, time);
		FlightData flightData;
		// initialize data packet
		flightData.flightStatus = true;
		flightData.fuelLevel = currentFuel;
		flightData.timeSinceEpoch = time;
		flightData.Length = 64;
		// only send the flightId for the first packet
		if (firstPacket == true)
		{
			flightData.flightId = flightID;
			firstPacket = false;
		}

		// Serialize
		bitstream stream = serializeFlightData(flightData);
		std::cout << "Time: " << time << std::endl;

		//FlightData newDat = deserializeFlightData(stream);
		//std::cout << flightData.flightId << " - " << newDat.flightId << "Fuel level" << std::endl;


		//transmissions->push_back(flightData);
		if (timeLast == 0) {
			timeLast = flightData.timeSinceEpoch;
		}

		// Wait for sim delay
		unsigned long long int simDelay = flightData.timeSinceEpoch - timeLast;
		std::cout << "delay: " << simDelay << std::endl;
		Sleep(simDelay * 1000);

		

		// Send stream
		c.send(stream);

		// Set timeLast
		timeLast = flightData.timeSinceEpoch;
	}

	//Send EOF signal -> data packet with flightStatus == false
	flightData = new FlightData();
	flightData->flightStatus = false;
	
	bitstream finalStream = serializeFlightData(*flightData);
	c.send(finalStream);
	//transmissions->push_back(flightData);
	*stop = true;
	//sendingThread->join();
	//delete[] sendingThread;
	//close connection

	readFile.close();
}

void ParseLine(std::string line, double& currentFuel, time_t& time)
{
	int pos = 0;
	std::string temp = "";

	std::cout << line << std::endl;

	try {
		//Remove the date portion from the string
		// 3_3_2023 14:53:21,4564.466309,
		pos = line.find("_");
		line = line.substr(pos);
		pos = line.find(" ");
		line = line.substr(pos + 1);

		//Seperate the fuel level from the time
		//14:53:21,4564.466309,
		pos = line.find(",");

		//temp = 4564.466309,
		temp = line.substr(pos + 1);

		// have line only contain time
		// line = 14:53:21
		line = line.substr(0, pos);

		//remove tail comma from fual value
		//temp = 4564.466309
		pos = temp.find(",");
		temp = temp.substr(0, pos);

		//convert temp string into double fuel value
		currentFuel = std::stod(temp);

		// convert string time into a time_t variable
		time = stringToTime(line);
	}
	catch (...)
	{
	}
}

time_t stringToTime(std::string line)
{
	std::tm timeStruct = {};

	// Parse the time string into the tm structure
	std::istringstream ss(line);
	ss >> std::get_time(&timeStruct, "%H:%M:%S");

	// Check for successful parsing
	if (ss.fail()) {
		std::cerr << "Failed to parse the time string." << std::endl;
	}

	// Set the date components to a valid default (e.g., January 1, 1970)
	timeStruct.tm_year = 70;
	timeStruct.tm_mon = 0;
	timeStruct.tm_mday = 1;

	// Convert tm structure to time_t
	time_t timeValue = std::mktime(&timeStruct);

	// Check if mktime was successful
	if (timeValue == -1) {
		std::cerr << "Failed to convert tm structure to time_t." << std::endl;

		// Check the tm structure
		std::cerr << "Invalid tm structure: "
			<< "Year: " << timeStruct.tm_year << " Month: " << timeStruct.tm_mon
			<< " Day: " << timeStruct.tm_mday << " Hour: " << timeStruct.tm_hour
			<< " Min: " << timeStruct.tm_min << " Sec: " << timeStruct.tm_sec << std::endl;
	}

	// check the time_t value
	std::cout << "Time in time_t format: " << timeValue << std::endl;

	return timeValue;
}