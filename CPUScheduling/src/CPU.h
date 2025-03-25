#pragma once

#include <vector>
#include <queue>
#include <algorithm>

#include "Process.h"

struct Stats {
	std::vector<Process> terminated;
	size_t time;
	size_t switchCount;
	size_t avgTAT;
	size_t avgWT;
};

class CPU {
private:
	Stats makeStats(const std::vector<Process>& terminated, size_t& time, size_t& switchCount) const {
		size_t avgTAT = 0;
		size_t avgWT = 0;
		for (const Process& p : terminated) {
			avgTAT += p.getTurnaroundTime();
			avgWT += p.getWaitingTime();
		}
		avgTAT /= terminated.size();
		avgWT /= terminated.size();
		return { terminated, time, switchCount, avgTAT, avgWT };
	}
public:
	CPU() {}
	~CPU() {}

	Stats FCFS(const std::vector<Process>& processes) const{
		std::queue<Process> readyQueue;
		std::vector<Process> terminated;
		terminated.reserve(processes.size());

		size_t time = -1;
		size_t switchCount = 0;

		auto it = processes.begin();

		while (!readyQueue.empty() || it != processes.end()) {
			time++;
			while (it != processes.end() && it->getArrivalTime() <= time) {
				readyQueue.push(*it);
				it++;
			}

			while (!readyQueue.empty()) {
				Process& current = readyQueue.front();
				time += current.getBurstTime();
				current.setEndTime(time);
				current.setTurnaroundTime(current.getEndTime() - current.getArrivalTime());
				current.setWaitingTime(current.getTurnaroundTime() - current.getBurstTime());
				terminated.push_back(current);
				readyQueue.pop();
				switchCount++;
			}
		}

		return makeStats(terminated, time, switchCount);
	}

	struct SJFComparator {
		bool operator()(const Process& a, const Process& b) {
			if (a.getBurstTime() == b.getBurstTime()) 
				return a.getPriority() < b.getPriority();
			return a.getBurstTime() > b.getBurstTime();
		}
	};

	Stats SJF(const std::vector<Process>& processes) const {
		std::priority_queue<Process, std::vector<Process>, SJFComparator> readyQueue;
		std::vector<Process> terminated;

		size_t time = 0;
		size_t switchCount = 0;

		auto it = processes.begin();

		const Process* current = nullptr;

		while (!readyQueue.empty() || it != processes.end()) {
			while (it != processes.end() && it->getArrivalTime() <= time) {
				readyQueue.push(*it);
				it++;
			}

			if (!readyQueue.empty()) {
				Process current = readyQueue.top();
				readyQueue.pop();

				time += current.getBurstTime();
				current.setEndTime(time);
				current.setTurnaroundTime(current.getEndTime() - current.getArrivalTime());
				current.setWaitingTime(current.getTurnaroundTime() - current.getBurstTime());

				terminated.push_back(current);
				switchCount++;
			}
			else {
				time++;
			}
		}

		return makeStats(terminated, time, switchCount);
	}

	Stats SJF_wyw(const std::vector<Process>& processes) const {
		std::priority_queue<Process, std::vector<Process>, SJFComparator> readyQueue;
		std::vector<Process> terminated;

		size_t time = 0;
		size_t switchCount = 0;

		auto it = processes.begin();

		while (!readyQueue.empty() || it != processes.end()) {
			while (it != processes.end() && it->getArrivalTime() <= time) {
				readyQueue.push(*it);
				it++;
			}
			if (!readyQueue.empty()) {
				Process tmp = readyQueue.top();
				readyQueue.pop(); // usuwam go z kolejki
				if (tmp.getProgress() >= tmp.getBurstTime()) {
					tmp.setEndTime(time);
					tmp.setTurnaroundTime(tmp.getEndTime() - tmp.getArrivalTime());
					tmp.setWaitingTime(tmp.getTurnaroundTime() - tmp.getBurstTime());
					terminated.push_back(tmp);
					switchCount++;
					continue;
				}
				else {
					tmp.incrementProgress();
				}
				readyQueue.push(tmp);
			}
			time++;
		}

		return makeStats(terminated, time, switchCount);
	}

	Stats RR(const std::vector<Process>& processes, const size_t quantum) const {
		std::queue<Process> readyQueue;
		std::vector<Process> terminated;
		terminated.reserve(processes.size());

		size_t time = 0;
		size_t switchCount = 0;

		auto it = processes.begin();

		while (!readyQueue.empty() || it != processes.end()) {
			while (it != processes.end() && it->getArrivalTime() <= time) {
				readyQueue.push(*it);
				it++;
			}

			if (!readyQueue.empty()) {
				Process current = readyQueue.front();
				readyQueue.pop();

				size_t step = std::min(quantum, current.getBurstTime() - current.getProgress());
				time += step;
				current.setProgress(current.getProgress() + step);

				if (current.getProgress() >= current.getBurstTime()) {
					current.setEndTime(time);
					current.setTurnaroundTime(current.getEndTime() - current.getArrivalTime());
					current.setWaitingTime(current.getTurnaroundTime() - current.getBurstTime());
					terminated.push_back(current);
				}
				else {
					readyQueue.push(current);
				}
				switchCount++;
			}
			else {
				time++;
			}
		}

		return makeStats(terminated, time, switchCount);
	}

};