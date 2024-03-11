





int main()
{

}

#include <vector>
#include <string>
#include "Parallel.h"
#include <atomic>
#include <windows.networking.sockets.h>
#include <iostream>
#include <string>

void listeningThread(std::shared_ptr<std::vector<std::pair<std::thread, ClientFlight*>>> flightRepository, bool* shutdown);

#pragma comment(lib, "Ws2_32.lib")
void main()
{
	bool shutdown = false;
	std::vector<std::pair<std::thread, ClientFlight*>> flights = std::vector<std::pair<std::thread, ClientFlight*>>();
	std::shared_ptr<std::vector<std::pair<std::thread, ClientFlight*>>> flightRepository = std::make_shared < std::vector<std::pair<std::thread, ClientFlight*>>>();
	std::thread listeningThread(listeningThread, flightRepository, &shutdown);
	//get a connection 

	while (getchar() == 0)
	{//do nothing
	}

	listeningThread.join();

}

void listeningThread(std::shared_ptr<std::vector<std::pair<std::thread, ClientFlight*>>> flightRepository, bool* shutdown)
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		return;

	SOCKET ServerSocket;
	ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (ServerSocket == INVALID_SOCKET) {
		WSACleanup();
		return;
	}

	sockaddr_in SvrAddr;
	SvrAddr.sin_family = AF_INET;
	SvrAddr.sin_addr.s_addr = INADDR_ANY;
	SvrAddr.sin_port = htons(27000);
	if (bind(ServerSocket, (struct sockaddr*)&SvrAddr, sizeof(SvrAddr)) == SOCKET_ERROR)
	{
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}

	if (listen(ServerSocket, 1) == SOCKET_ERROR) {
		closesocket(ServerSocket);
		WSACleanup();
		return;
	}

	std::cout << "Waiting for client connection\n" << std::endl;

	SOCKET ConnectionSocket;

	while (!shutdown)
	{
		ConnectionSocket = SOCKET_ERROR;
		if ((ConnectionSocket = accept(ServerSocket, NULL, NULL)) == SOCKET_ERROR) {
			closesocket(ServerSocket);
			WSACleanup();
			return;
		}

		std::cout << "Connection Established" << std::endl;

		char* buffer = new char[1024];
		memset(buffer, 0, 1024);
		//determine the planeID and dispatch a thread to continue accepting data
		recv(ConnectionSocket, buffer, 1024, 0);

		ClientFlight* planeFlight = new ClientFlight();

		memcpy(&planeFlight, buffer, sizeof(ClientFlight));

		std::thread planeConnection(activeFlight, ConnectionSocket, planeFlight);
		//push back never changes the index of the currently held flight connections
		flightRepository->push_back(std::make_pair(planeConnection, planeFlight));

		for (int i = 0; i < flightRepository->size(); i++)
		{
			//only ever reading critical data, never writing
			//using a shared ptr anyways
			if (flightRepository->at(i).second->getFlightStatus() == false)
			{
				flightRepository->at(i).first.join();

				//write to disk the second in the pair



				//free memory
				delete flightRepository->at(i).second;
			}
		}
	}

	for (int i = 0; i < flightRepository->size(); i++)
	{
		flightRepository->at(i).first.join();
	}

}

void activeFlight(SOCKET ConnectionSocket, std::atomic<ClientFlight*> data)
{
	double total = 0;
	double timespan = 0;
	char* buffer = new char[1024];
	memset(buffer, 0, 1024);

	//calculation
	bool flightStatus = data.load(std::memory_order_relaxed)->getFlightStatus();


	recv(ConnectionSocket, buffer, 1024, 0);
	ClientFlightData dataRecevied;
	std::memcpy(buffer, &dataRecevied, sizeof(ClientFlightData));

	total += dataRecevied.fuelLevel;
	timespan += dataRecevied.

}