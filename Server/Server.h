#pragma once
#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include "DataStorage.h"  // Make sure this is the correct path to DataStorage.h
#include "DataAnalytics.h"
#pragma comment(lib, "Ws2_32.lib")

using std::string;
using std::cout;
using std::endl;
using std::thread;

class Server {
private:
	bool debug = false;
	bool printData = true;
	bool clearScreen = true;

	int serverPort = 12345;
	DataStorage& storage;  // Reference to a DataStorage object
	std::thread displayThread;

	void startDisplayThread();

public:
	// Constructor to initialize the DataStorage reference
	Server(DataStorage& ds) : storage(ds) {
		startDisplayThread();
	}

	void startServer();

	void handleClient(SOCKET clientSocket);
};