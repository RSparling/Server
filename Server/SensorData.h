#pragma once
#include <ctime> // time_t

// Data structure for storing sensor readings
struct SensorData {
	time_t timestamp; // Declare the timestamp field
	float temperature;
	float humidity;

	// Constructor that takes timestamp, temperature, and humidity
	SensorData(time_t ts, float temp, float humi) : timestamp(ts), temperature(temp), humidity(humi) {
	}
};