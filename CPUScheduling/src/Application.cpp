#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

#include "ProcessGenerator.h"
#include "CPU.h"





static void print(const std::vector<Process>& processes) {
	for (const Process& process : processes) {
		std::cout << process.toString() << std::endl;
	}
}

static void saveStats(const Stats& stats, const std::string& filename) {
	//std::filesystem::create_directories("graphs");
	std::ofstream file("graphs/" + filename);

	if (!file) {
		throw std::ios_base::failure("Blad otwarcia pliku: graphs/" + filename);
	}

	file << stats.time << " " << stats.switchCount << " " << stats.avgTAT << " " << stats.avgWT << "\n";

	for (const Process& p : stats.terminated) {
		file << p.getId() << " " << p.getArrivalTime() << " " << p.getBurstTime() << " " << p.getPriority() << " " << p.getEndTime() << " " << p.getWaitingTime() << " " << p.getTurnaroundTime() << "\n";
	}

	file.close();
	std::cerr << "Statystyki zostaly poprawnie zapisane do graphs/" + filename << std::endl;
}

static std::string modeToStr(Mode mode) {
	switch (mode)
	{
	case GAUSSIAN:
		return "gaussian";
	case LINEAR:
		return "linear";
	case INVERSE_LINEAR:
		return "inverse_linear";
	default:
		return "error";
	}
}

static size_t countStarved(size_t avgWT, std::vector<Process>& terminated, size_t starvationFactor) {
	size_t count = 0;
	for (const Process& p : terminated) {
		if (p.getWaitingTime() > starvationFactor * avgWT) count++;
	}
	return count;
}

static void generateTests(int tests, Mode ModeArrivalTime, Mode ModeBurstTime) {
	const size_t count = 1000;
	const double maxBurstTime = 100.0;
	const double minBurstTime = 20.0;
	const double maxArrivalTime = 100000.0;
	const double minArrivalTime = 0.0;
	const size_t quant = 50;
	const size_t starvationFactor = 6;

	size_t avgFCFS_TAT = 0;
	size_t avgFCFS_WT = 0;
	size_t avgSJF_TAT = 0;
	size_t avgSJF_WT = 0;
	size_t avgSJF_wyw_TAT = 0;
	size_t avgSJF_wyw_WT = 0;
	size_t avgRR_TAT = 0;
	size_t avgRR_WT = 0;

	size_t starvedFCFS = 0;
	size_t starvedSJF = 0;
	size_t starvedSJF_wyw = 0;
	size_t starvedRR = 0;

	size_t switchFCFS = 0;
	size_t switchSJF = 0;
	size_t switchSJF_wyw = 0;
	size_t switchRR = 0;

	std::vector<Mode> modes = { Mode::GAUSSIAN, Mode::LINEAR, Mode::INVERSE_LINEAR };
	for (int i = 1; i <= tests; i++) {
		ProcessGenerator generator;
		std::vector<Process> processes = generator.generateProcesses(count, ModeArrivalTime, minArrivalTime, maxArrivalTime, ModeBurstTime, minBurstTime, maxBurstTime, {1, 2, 3, 4});
		std::ofstream file("graphs/test_" + std::to_string(i) + "_" + modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + ".txt");
		if (!file) {
			throw std::ios_base::failure("Blad otwarcia pliku: graphs/test_" + std::to_string(i) +"_" + modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + ".txt");
		}
		for (const Process& p : processes) {
			file << p.getId() << " " << p.getArrivalTime() << " " << p.getBurstTime() << " " << p.getPriority() << " " << p.getEndTime() << " " << p.getWaitingTime() << " " << p.getTurnaroundTime() << "\n";
		}
		file.close();
		CPU cpu;
		Stats statsFCFS = cpu.FCFS(processes);
		Stats statsSJF = cpu.SJF(processes);
		Stats statsSJF_wyw = cpu.SJF_wyw(processes);
		Stats statsRR = cpu.RR(processes, quant);
		avgFCFS_TAT += statsFCFS.avgTAT;
		avgFCFS_WT += statsFCFS.avgWT;
		avgSJF_TAT += statsSJF.avgTAT;
		avgSJF_WT += statsSJF.avgWT;
		avgSJF_wyw_TAT += statsSJF_wyw.avgTAT;
		avgSJF_wyw_WT += statsSJF_wyw.avgWT;
		avgRR_TAT += statsRR.avgTAT;
		avgRR_WT += statsRR.avgWT;
		saveStats(statsFCFS, modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + "_FCFS_" + std::to_string(i) + ".txt");
		saveStats(statsSJF, modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) +  "_SJF_" + std::to_string(i) + ".txt");
		saveStats(statsSJF_wyw, modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + "_SJF_wyw_" + std::to_string(i) + ".txt");
		saveStats(statsRR, modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + "_RR_" + std::to_string(i) + ".txt");
		std::cerr << "Test " << i << " zostal wygenerowany" << std::endl;
		starvedFCFS += countStarved(avgFCFS_WT, statsFCFS.terminated, starvationFactor);
		starvedSJF += countStarved(avgSJF_WT, statsSJF.terminated, starvationFactor);
		starvedSJF_wyw += countStarved(avgSJF_wyw_WT, statsSJF_wyw.terminated, starvationFactor);
		starvedRR += countStarved(avgRR_WT, statsRR.terminated, starvationFactor);
		switchFCFS += statsFCFS.switchCount;
		switchSJF += statsSJF.switchCount;
		switchSJF_wyw += statsSJF_wyw.switchCount;
		switchRR += statsRR.switchCount;
	}
	avgFCFS_TAT /= tests;
	avgFCFS_WT /= tests;
	avgSJF_TAT /= tests;
	avgSJF_WT /= tests;
	avgSJF_wyw_TAT /= tests;
	avgSJF_wyw_WT /= tests;
	avgRR_TAT /= tests;
	avgRR_WT /= tests;
	starvedFCFS /= tests;
	starvedSJF /= tests;
	starvedSJF_wyw /= tests;
	starvedRR /= tests;
	switchFCFS /= tests;
	switchSJF /= tests;
	switchSJF_wyw /= tests;
	switchRR /= tests;
	std::cerr << "Wyniki dla trybu pojawiania sie procesow: " + modeToStr(ModeArrivalTime) + ", tryb trwania procesow: " + modeToStr(ModeBurstTime) << std::endl;
	std::cerr << "Sredni czas oczekiwania dla FCFS: " << avgFCFS_WT << std::endl;
	std::cerr << "Sredni czas obrotu dla FCFS: " << avgFCFS_TAT << std::endl;
	std::cerr << "Sredni czas oczekiwania dla SJF: " << avgSJF_WT << std::endl;
	std::cerr << "Sredni czas obrotu dla SJF: " << avgSJF_TAT << std::endl;
	std::cerr << "Sredni czas oczekiwania dla SJF_wyw: " << avgSJF_wyw_WT << std::endl;
	std::cerr << "Sredni czas obrotu dla SJF_wyw: " << avgSJF_wyw_TAT << std::endl;
	std::cerr << "Sredni czas oczekiwania dla RR: " << avgRR_WT << std::endl;
	std::cerr << "Sredni czas obrotu dla RR: " << avgRR_TAT << std::endl;
	std::ofstream file("graphs/wyniki_" + modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + ".txt");
	if (!file) {
		throw std::ios_base::failure("Blad otwarcia pliku: graphs / wyniki_" + modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + ".txt");
	}
	file << avgFCFS_WT << " " << avgFCFS_TAT << " " << avgSJF_WT << " " << avgSJF_TAT << " " << avgSJF_wyw_WT << " " << avgSJF_wyw_TAT << " " << avgRR_WT << " "<< avgRR_TAT << std::endl;
	file.close();
	std::ofstream file1("graphs/starvation_" + modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + ".txt");
	if (!file1) {
		throw std::ios_base::failure("Blad otwarcia pliku: graphs / wyniki_" + modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + ".txt");
	}
	file1 << starvedFCFS << " " << starvedSJF << " " << starvedSJF_wyw << " " << starvedRR << std::endl;
	file1.close();
	std::ofstream file2("graphs/switch_" + modeToStr(ModeArrivalTime) + "_" + modeToStr(ModeBurstTime) + ".txt");
	file2 << switchFCFS << " " << switchSJF << " " << switchSJF_wyw << " " << switchRR;
	file2.close();
	std::cerr << std::endl;
}

int main() {
	std::ios::sync_with_stdio(0);
	std::cout.tie(0);
	
	const int tests = 5;
	
	generateTests(tests, Mode::GAUSSIAN, Mode::GAUSSIAN);
	generateTests(tests, Mode::GAUSSIAN, Mode::LINEAR);
	generateTests(tests, Mode::GAUSSIAN, Mode::INVERSE_LINEAR);
	generateTests(tests, Mode::LINEAR, Mode::GAUSSIAN);
	generateTests(tests, Mode::LINEAR, Mode::LINEAR);
	generateTests(tests, Mode::LINEAR, Mode::INVERSE_LINEAR);
	generateTests(tests, Mode::INVERSE_LINEAR, Mode::GAUSSIAN);
	generateTests(tests, Mode::INVERSE_LINEAR, Mode::LINEAR);
	generateTests(tests, Mode::INVERSE_LINEAR, Mode::INVERSE_LINEAR);

	return 0;
}