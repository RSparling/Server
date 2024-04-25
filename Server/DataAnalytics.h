#pragma once
#include <iostream>
#include <numeric>
#include <vector>
#include <ctime>
#include <limits>
#include "DataStorage.h"

class DataAnalytics {
private:
	DataStorage& storage;

	float computeAverage(const std::vector<SensorData>& data, float SensorData::* value) {
		if (data.empty()) return std::numeric_limits<float>::quiet_NaN();  // Return NaN if no data available

		float sum = std::accumulate(data.begin(), data.end(), 0.0f, [value](float acc, const SensorData& sd) {
			return acc + sd.*value;
			});
		return sum / data.size();
	}

public:
	DataAnalytics(DataStorage& ds) : storage(ds) {}

	void displayHourlyAverages() {
		time_t now = time(nullptr);
		time_t currentHour = now - (now % 3600);  // Round down to the nearest hour

		std::vector<std::string> outputs;  // To store the outputs for each hour
		bool anyDataAvailable = false;  // Flag to check if any hour has data

		for (int i = 23; i >= 0; i--) {
			time_t startTime = currentHour - i * 3600;  // Calculate start time for each hour
			auto data = storage.getDataForHour(startTime);
			float avgTemperature = computeAverage(data, &SensorData::temperature);
			float avgHumidity = computeAverage(data, &SensorData::humidity);

			struct tm ptm;
			char buffer[10];
			localtime_s(&ptm, &startTime);
			strftime(buffer, sizeof(buffer), "%Hh", &ptm);  // Format time as "HHh"

			if (!std::isnan(avgTemperature) && !std::isnan(avgHumidity)) {
				outputs.push_back(std::string(buffer) + "\n- " + std::to_string(avgTemperature) + " C, " + std::to_string(avgHumidity) + "% Humidity");
				anyDataAvailable = true;
			}
		}

		std::cout << "Hourly averages for the last 24 hours:\n";
		if (anyDataAvailable) {
			for (const auto& output : outputs) {
				std::cout << output << std::endl;
			}
		}
		else {
			std::cout << "Previous hours unavailable\n";
		}
	}
};
