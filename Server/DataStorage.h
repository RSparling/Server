#pragma once
#include <vector>
#include <mutex>
#include <algorithm>
#include <ctime>
#include <iterator>
#include <iostream>
#include "SensorData.h"

using std::vector;
using std::mutex;
using std::lock_guard;

class DataStorage {
private:
	vector<SensorData> data;
	mutex mtx;

public:
	void addData(const SensorData& newData) {
		if (isValid(newData)) {
			lock_guard<mutex> guard(mtx);
			data.push_back(newData);
			// Clean up old data if necessary
			time_t now = time(nullptr);
			data.erase(remove_if(data.begin(), data.end(), [now](const SensorData& sd) {
				return now - sd.timestamp > 86400;  // 86400 seconds in a day
				}), data.end());
		}
		else {
			//std::cout << "Invalid data - Temperature: " << newData.temperature << ", Humidity: " << newData.humidity << std::endl;
		}
	}

	bool isValid(const SensorData& data) {
		return (data.temperature >= -50 && data.temperature <= 50) &&
			(data.humidity >= 0 && data.humidity <= 100);
	}

	std::vector<SensorData> getDataForLastHour() {
		std::lock_guard<std::mutex> guard(mtx);
		time_t now = std::time(nullptr);
		std::vector<SensorData> result;
		std::copy_if(data.begin(), data.end(), std::back_inserter(result), [now](const SensorData& sd) {
			return now - sd.timestamp <= 3600;  // 3600 seconds in an hour
			});
		return result;
	}

	std::vector<SensorData> getDataForHour(time_t startTime) {
		std::lock_guard<std::mutex> guard(mtx);
		std::vector<SensorData> result;
		std::copy_if(data.begin(), data.end(), std::back_inserter(result), [startTime](const SensorData& sd) {
			return sd.timestamp >= startTime && sd.timestamp < startTime + 3600;  // Data within one hour
			});
		return result;
	}

	vector<SensorData> getDataForLast24Hours() {
		lock_guard<mutex> guard(mtx);
		time_t now = time(nullptr);
		vector<SensorData> result;
		copy_if(data.begin(), data.end(), back_inserter(result), [now](const SensorData& sd) {
			return now - sd.timestamp <= 86400;
			});
		return result;
	}
};