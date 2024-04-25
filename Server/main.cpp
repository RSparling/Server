#include "DataStorage.h"  // Include your DataStorage header
#include "Server.h"       // Include your Server header
#include "DataAnalytics.h"

int main() {
	DataStorage storage;

	Server server(storage);
	server.startServer();
	return 0;
}