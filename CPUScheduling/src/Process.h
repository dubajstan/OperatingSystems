#pragma once

#include <string>

using std::string, std::to_string;

class Process {
private:
	size_t id;
	size_t arrivalTime;
	size_t burstTime;
	size_t priority;
	size_t endTime;
	size_t waitingTime;
	size_t turnaroundTime;
	size_t progress;

public:
	Process(size_t id, size_t priority, size_t arrivalTime, size_t burstTime)
		:id(id), priority(priority), arrivalTime(arrivalTime), burstTime(burstTime), progress(0), endTime(0), waitingTime(0), turnaroundTime(0)
	{
	}

	~Process() {}

	inline size_t getId() const {
		return id;
	}
	inline size_t getArrivalTime() const {
		return arrivalTime;
	}
	inline size_t getBurstTime() const {
		return burstTime;
	}
	inline size_t getPriority() const {
		return priority;
	}
	inline size_t getEndTime() const {
		return endTime;
	}
	inline size_t getWaitingTime() const {
		return waitingTime;
	}
	inline size_t getTurnaroundTime() const {
		return turnaroundTime;
	}
	inline size_t getProgress() const {
		return progress;
	}

	inline void setEndTime(size_t endTime) {
		this->endTime = endTime;
	}
	inline void setWaitingTime(size_t waitingTime) {
		this->waitingTime = waitingTime;
	}
	inline void setTurnaroundTime(size_t turnaroundTime) {
		this->turnaroundTime = turnaroundTime;
	}
	inline void setProgress(size_t progress) {
		this->progress = progress;
	}

	inline void incrementProgress() {
		progress++;
	}

	std::string toString() const {
		return "Proces " + std::to_string(id) +
			" arrival time: " + std::to_string(arrivalTime) +
			" end time: " + std::to_string(endTime) +
			" burst time: " + std::to_string(burstTime) +
			" priority: " + std::to_string(priority) +
			" waiting time: " + std::to_string(waitingTime) +
			" turn around time: " + std::to_string(turnaroundTime) +
			" progress: " + std::to_string(progress);
	}
};