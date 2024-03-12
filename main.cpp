#pragma once
#include <vector>
#include <string>
#include "Flight.h"
#include <atomic>
#include <windows.networking.sockets.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <ctime>
#include <filesystem>
#include <iomanip>
//#include "ConfigReader.h"

#pragma comment(lib, "Ws2_32.lib")

time_t stringToTime(std::string line);

void main() {
	// Open connection / Connect to server

	std::cout << "Connected to server" << std::endl;

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
	srand(time(0));
	int randomNumber = minRange + std::rand() % (maxRange - minRange + 1);

	// Open selected randomized file
	std::ifstream readFile(flightRecords[randomNumber]);
	std::cout << "file opened: " << flightRecords[randomNumber] << std::endl;

	std::string line = "";
	std::string temp = "";
	time_t time;
	double currentFuel = 0;
	int pos = 0;

	// Get the first line which we dont need for the circumstance and ignore it
	std::getline(readFile, line);

	std::getline(readFile, line);
	std::cout << "line read:" << line << std::endl;

	//Remove the date portion from the string
	// 3_3_2023 14:53:21,4564.466309,

	pos = line.find("_");
	line = line.substr(pos);

	pos = line.find(" ");
	line = line.substr(pos + 1);
	std::cout << "Date Removed:" << line << std::endl;

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

	std::cout << "Current Fuel:" << temp << "     Time:" << line << std::endl;

	//convert temp string into fuel value
	currentFuel = std::stod(temp);

	// convert string time into a time_t variable
	time = stringToTime(line);

	std::cout << "Current Fuel: " << currentFuel << "     Time: " << time << std::endl;

	// loop and read file until the EOF
	while (std::getline(readFile, line);)
	{
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

		std::cout << "Current Fuel: " << currentFuel << "     Time: " << time << std::endl;

		// initialize data packet
		// add condition to only send the id for the first packet
	}
	//Send EOF signal -> data packet with flightStatus == false

	//close connection

	//End of Program
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