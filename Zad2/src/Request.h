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
	bool isStarved;

public:
	Request(size_t id, size_t position, size_t arrival_Time, size_t deadline, bool isRealTime);
	~Request() = default;

	size_t getId() const;
	size_t getPosition() const;
	size_t getArrivalTime() const;
	size_t getWaitTime() const;
	size_t getDeadline() const;
	bool getIsRealTime() const;
	bool getIsStarved() const;
	size_t getServiceTime() const;

	void setWaitTime(size_t wait_Time);
	void setIsStarved(bool isStarved);

	bool operator==(const Request& other) const {
		return id == other.id && position == other.position;
	}

	std::string toString() const;
};