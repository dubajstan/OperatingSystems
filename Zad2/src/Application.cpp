#include <iostream>
#include <vector>
#include "RequestGenerator.h"
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include "DiscScheduler.h"

const size_t disc_Size = 500;
const size_t time_Limit = 15000;
const size_t request_Count = 1000;
const std::vector<std::pair<double, double>> ranges = { {0.0, 0.15} }; //!!!!!! POKAZAC RAZ DLA PELNEGO ZBIORU I DLA MALEGO -> PRZEWAGA SSTF
const double real_Time_Chance = 0.4;
const size_t d_Min = 150;
const size_t d_Max = 550;
const size_t start_Position = 60;

std::vector<Request> createRequests() {
	std::vector<Request> requests;

	// addRT(id, position, arrival_Time, deadline) - isRealTime = true
	requests.emplace_back(1, 120, 3, 120, true);
	requests.emplace_back(2, 70, 3, 50, true);
	requests.emplace_back(3, 80, 6, 30, true);
	requests.emplace_back(4, 170, 7, 110, true);
	requests.emplace_back(5, 75, 11, 14, true);
	requests.emplace_back(6, 86, 14, 10, true);
	requests.emplace_back(7, 113, 15, 20, true);

	// add(id, position, arrival_Time) - isRealTime = false, deadline = 0 (lub inne domyœlne)
	requests.emplace_back(8, 14, 4, 0, false);
	requests.emplace_back(9, 37, 5, 0, false);
	requests.emplace_back(10, 183, 8, 0, false);
	requests.emplace_back(11, 54, 9, 0, false);
	requests.emplace_back(12, 55, 10, 0, false);
	requests.emplace_back(13, 14, 12, 0, false);
	requests.emplace_back(14, 80, 13, 0, false);
	requests.emplace_back(15, 183, 16, 0, false);

	return requests;
}

static void saveRequestsToCSV(const std::vector<Request>& requests, std::string filepath) {
	std::ofstream file(filepath);

	if (!file.is_open()) {
		std::cerr << "Nie mozna otworzyc pliku: " << filepath << std::endl;
		return;
	}

	file << "ID,ArrivalTime,Position,IsRealTime,Deadline,WaitTime,IsStarved,ServiceTime\n"; //naglowki

	for (const Request& r : requests) {
		file << r.getId() << ","
			<< r.getArrivalTime() << ","
			<< r.getPosition() << ","
			<< (r.getIsRealTime() ? "True" : "False") << ","
			<< r.getDeadline() << ","
			<< r.getWaitTime() << ","
			<< (r.getIsStarved() ? "True" : "False") << ","
			<< r.getServiceTime() << "\n";
	}

	file.close();
	std::cout << "Zapisano dane do pliku: " << filepath << std::endl;
}

std::vector<Request> test(int num, DiscScheduler& scheduler, std::vector<Request>& vec) {
	switch (num) {
	case 0:
		return scheduler.fcfs(vec);
	case 1:
		return scheduler.sstf(vec);
	case 2:
		return scheduler.scan(vec);
	case 3:
		return scheduler.cscan(vec);
	case 4:
		return scheduler.sstf_edf(vec);
	case 5:
		return scheduler.sstf_fd_scan(vec);
	default:
		throw std::runtime_error("Illegal argument");
	}
}

std::string getAlg(int num) {
	switch (num) {
	case 0:
		return "fcfs";
	case 1:
		return "sstf";
	case 2:
		return "scan";
	case 3:
		return "cscan";
	case 4:
		return "sstf_edf";
	case 5:
		return "sstf_fd_scan";
	default:
		throw std::runtime_error("Illegal argument");
	}
}

void testExample() {
	std::vector<Request> requests = createRequests();

	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });

	DiscScheduler scheduler(disc_Size, start_Position, time_Limit, request_Count);
	for (int i = 0; i < 6; i++) {
		std::vector<Request> completed = test(i, scheduler, requests);
		saveRequestsToCSV(completed, "data/testExample_" + getAlg(i)+".csv");
	}

	std::cout << "Utworzono wykresy do testExample" << std::endl;
}

std::string getMode(Mode a) {
	switch (a)
	{
	case NORMAL:
		return "normal";
	case UNIFORM:
		return "uniform";
	default:
		break;
	}
}

void test1(Mode position_mode,Mode time_mode, bool isRT) {
	RequestGenerator generator(1);
	std::vector<Request> requests = generator.generateRequests(disc_Size, time_Limit, request_Count, ranges, position_mode, time_mode, isRT, real_Time_Chance, d_Min, d_Max, false);

	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });

	DiscScheduler scheduler(disc_Size, start_Position, time_Limit, request_Count);
	if (isRT) {
		for (int i = 4; i < 6; i++) {
			std::vector<Request> completed = test(i, scheduler, requests);
			saveRequestsToCSV(completed, "data/test_" + getMode(position_mode) + "_" + getMode(time_mode) + "_" + getAlg(i)+".csv");
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			std::vector<Request> completed = test(i, scheduler, requests);
			saveRequestsToCSV(completed, "data/test_" + getMode(position_mode) + "_" + getMode(time_mode) + "_" + getAlg(i)+".csv");
		}
	}

	std::cout << "Utworzono wykresy" << std::endl;
}

void testAntyScan(Mode position_mode, Mode time_mode, bool isRT) {
	RequestGenerator generator(1);
	std::vector<Request> requests = generator.generateRequests(disc_Size, time_Limit, request_Count, ranges, position_mode, time_mode, isRT, real_Time_Chance, d_Min, d_Max, true);

	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });

	DiscScheduler scheduler(disc_Size, start_Position, time_Limit, request_Count);
	if (isRT) {
		for (int i = 4; i < 6; i++) {
			std::vector<Request> completed = test(i, scheduler, requests);
			saveRequestsToCSV(completed, "data/testSCAN_" + getMode(position_mode) + "_" + getMode(time_mode) + "_" + getAlg(i) + ".csv");
		}
	}
	else {
		for (int i = 0; i < 4; i++) {
			std::vector<Request> completed = test(i, scheduler, requests);
			saveRequestsToCSV(completed, "data/testSCAN_" + getMode(position_mode) + "_" + getMode(time_mode) + "_" + getAlg(i) + ".csv");
		}
	}

	std::cout << "Utworzono wykresy" << std::endl;
}






int main() {
	const Mode postion_Mode = Mode::UNIFORM;
	const Mode arrival_Time_Mode = Mode::UNIFORM;
	const bool isRealTime = true;
	

	//const std::vector<std::pair<double, double>> ranges2 = { {0.6, 1.0} };

	//RequestGenerator generator(1);
	//std::vector<Request> requests = generator.generateRequests(disc_Size, time_Limit, request_Count, ranges, postion_Mode, arrival_Time_Mode, isRealTime, real_Time_Chance, d_Min, d_Max);
	//std::vector<Request>tmp = generator.generateRequests(disc_Size, time_Limit, request_Count, ranges2, postion_Mode, arrival_Time_Mode, isRealTime, real_Time_Chance, d_Min, d_Max);
	//requests.insert(requests.begin(), tmp.begin(), tmp.end());
	// najpier wejdzie do 122 -> 183 -> 124 -> 98 -> ...
	//std::vector<Request> requests = createRequests();

	//std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });

	//DiscScheduler scheduler(disc_Size, start_Position, time_Limit, request_Count);
	//std::vector<Request> completed = scheduler.sstf_fd_scan(requests);
	//saveRequestsToCSV(completed, "data/requests.csv");
	

	// przykladowy
	testExample();
	// bez real time
	test1(Mode::UNIFORM, Mode::UNIFORM, false);
	test1(Mode::NORMAL, Mode::UNIFORM, false);
	test1(Mode::UNIFORM, Mode::NORMAL, false);
	test1(Mode::NORMAL, Mode::NORMAL, false);
	// z real time
	test1(Mode::UNIFORM, Mode::UNIFORM, true);
	test1(Mode::NORMAL, Mode::NORMAL, true);
	test1(Mode::NORMAL, Mode::UNIFORM, true);
	test1(Mode::UNIFORM, Mode::NORMAL, true);

	testAntyScan(Mode::UNIFORM, Mode::UNIFORM, false);

	system("python scripts/results.py");
	std::cout << "Stworzono wykresy" << std::endl;

	return 0;
}