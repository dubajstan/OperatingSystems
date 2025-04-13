#include <iostream>
#include <vector>
#include "RequestGenerator.h"
#include <algorithm>
#include <fstream>

void saveRequestsToCSV(const std::vector<Request>& requests, std::string filepath) {
	std::ofstream file(filepath);

	if (!file.is_open()) {
		std::cerr << "Nie mozna otworzyc pliku: " << filepath << std::endl;
		return;
	}

	file << "ID,ArrivalTime,Position,IsRealTime,Deadline,WaitTime\n"; //naglowki

	for (const Request& r : requests) {
		file << r.getId() << ","
			<< r.getArrivalTime() << ","
			<< r.getPosition() << ","
			<< (r.getIsRealTime() ? "true" : "false") << ","
			<< r.getDeadline() << ","
			<< r.getWaitTime() << "\n";
	}

	file.close();
	std::cout << "Zapisano dane do pliku: " << filepath << std::endl;
}


int main() {
	const size_t disc_Size = 20;
	const size_t time_Limit = 30;
	const size_t request_Count = 15;
	const std::vector<std::pair<double, double>> ranges = { {0.15, 0.40},{0.8, 1.0} };
	const Mode postion_Mode = Mode::NORMAL;
	const Mode arrival_Time_Mode = Mode::UNIFORM;
	const bool isRealTime = true;
	const double real_Time_Chance = 0.25;
	const size_t d_Min = 0;
	const size_t d_Max = 50;

	RequestGenerator generator(1);
	std::vector<Request> requests = generator.generateRequests(disc_Size, time_Limit, request_Count, ranges, postion_Mode, arrival_Time_Mode, isRealTime, real_Time_Chance, d_Min, d_Max);
	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });

	saveRequestsToCSV(requests, "requests.cvs");

	return 0;
}