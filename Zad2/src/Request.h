#pragma once
#include <string>
#include <sstream>

class Request {
private:
	size_t id;
	size_t position;
	size_t arrival_Time;
	size_t wait_Time;
	size_t deadline;
	bool isRealTime;

public:
	Request(size_t id, size_t position, size_t arrival_Time, size_t deadline, bool isRealTime);
	~Request();

	size_t getId() const;
	size_t getPosition() const;
	size_t getArrivalTime() const;
	size_t getWaitTime() const;
	size_t getDeadline() const;
	bool getIsRealTime() const;

	void setWaitTime(size_t wait_Time);

	std::string toString() const;
};