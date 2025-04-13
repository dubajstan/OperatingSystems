#include "Request.h"

Request::Request(size_t id, size_t position, size_t arrival_Time, size_t deadline, bool isRealTime)
	: id(id), position(position), arrival_Time(arrival_Time), deadline(deadline), isRealTime(isRealTime), wait_Time(0)
{}

Request::~Request() {}

size_t Request::getId() const {
	return id;
}

size_t Request::getPosition() const {
	return position;
}

size_t Request::getArrivalTime() const {
	return arrival_Time;
}

size_t Request::getWaitTime() const {
	return wait_Time;
}

size_t Request::getDeadline() const {
	return deadline;
}

bool Request::getIsRealTime() const {
	return isRealTime;
}

void Request::setWaitTime(size_t wait_Time) {
	this->wait_Time = wait_Time;
}

std::string Request::toString() const{
	std::ostringstream oss;
	oss << "ID: " << id
		<< ", Position: " << position
		<< ", Arrival Time: " << arrival_Time
		<< ", Wait Time: " << wait_Time
		<< ", Deadline: " << deadline
		<< ", Real-time: " << (isRealTime ? "Yes" : "No");
	return oss.str();
}
