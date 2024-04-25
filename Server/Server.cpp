#include "Server.h"

void Server::startServer() {
	WSADATA wsaData;
	SOCKET serverSocket, clientSocket;
	struct sockaddr_in server, client;

	cout << "Initialising Winsock..." << endl;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		if (debug)
			cout << "Failed. Error Code : " << WSAGetLastError();
		return;
	}

	// Create a socket
	if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		if (debug)
			cout << "Could not create socket : " << WSAGetLastError();
	}

	// Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(serverPort);

	// Bind
	if (bind(serverSocket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
		if (debug)
			cout << "Bind failed with error code : " << WSAGetLastError();
		exit(EXIT_FAILURE);
	}

	// Listen to incoming connections
	listen(serverSocket, 3);

	// Accept an incoming connection
	cout << "Waiting for incoming connections..." << endl;

	int c = sizeof(struct sockaddr_in);
	while ((clientSocket = accept(serverSocket, (struct sockaddr*)&client, &c)) != INVALID_SOCKET) {
		if (debug)
			cout << "Connection accepted." << endl;
		thread clientThread(&Server::handleClient, this, clientSocket);
		clientThread.detach();
	}

	if (clientSocket == INVALID_SOCKET) {
		if (debug)
			cout << "accept failed with error code : " << WSAGetLastError();
		return;
	}

	// Close server socket and clean up Winsock
	closesocket(serverSocket);
	WSACleanup();
}

void Server::handleClient(SOCKET clientSocket) {
	{
		char buffer[1024];
		int receivedBytes;

		while ((receivedBytes = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
			buffer[receivedBytes] = '\0';  // Ensure null-termination for string processing
			if (debug)
				std::cout << "Received data: " << buffer << std::endl;

			// Assuming buffer contains data formatted as: "uptimeMillis,temperature,humidity"
			unsigned long long uptimeMillis;
			float temperature, humidity;
			sscanf_s(buffer, "%llu,%f,%f", &uptimeMillis, &temperature, &humidity);

			// Calculate the timestamp
			auto now = std::chrono::system_clock::now();
			auto uptimeDuration = std::chrono::milliseconds(uptimeMillis);
			auto measuredTime = std::chrono::system_clock::time_point(now - uptimeDuration);
			time_t measuredTimeT = std::chrono::system_clock::to_time_t(measuredTime);

			// Now you can store the data with the correct timestamp
			storage.addData(SensorData(measuredTimeT, temperature, humidity));

			// Optionally, you can send a response back to the client
			string response = "Data received\n";
			send(clientSocket, response.c_str(), response.length(), 0);
		}

		if (receivedBytes == 0) {
			if (debug)
				std::cout << "Client disconnected." << std::endl;
		}
		else {
			if (debug)
				std::cout << "recv failed: " << WSAGetLastError() << std::endl;
		}

		closesocket(clientSocket);
	}
}

void Server::startDisplayThread() {
	displayThread = std::thread([this]() {
		DataAnalytics analytics(this->storage);
		while (true) {  // Loop until the server shuts down
			if (clearScreen)
				system("cls");

			if (printData)
				analytics.displayHourlyAverages();
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));  // Display every hour
		}
		});
	displayThread.detach();  // Detach the thread to run independently
}