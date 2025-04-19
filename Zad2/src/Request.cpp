#include "Request.h"

Request::Request(size_t id, size_t position, size_t arrival_Time, size_t deadline, bool isRealTime)
	: id(id), position(position), arrival_Time(arrival_Time), deadline(deadline), isRealTime(isRealTime), wait_Time(0), isStarved(false)
{}

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

bool Request::getIsStarved() const {
	return getServiceTime() > arrival_Time + deadline;
}

size_t Request::getServiceTime() const {
	return wait_Time + arrival_Time;
}

void Request::setWaitTime(size_t wait_Time) {
	this->wait_Time = wait_Time;
}

void Request::setIsStarved(bool isStarved) {
	this->isStarved = isStarved;
}

std::string Request::toString() const{
	std::ostringstream oss;
	oss << "ID: " << id
		<< ", Position: " << position
		<< ", Arrival Time: " << arrival_Time
		<< ", Wait Time: " << wait_Time
		<< ", Deadline: " << deadline
		<< ", Real-time: " << (isRealTime ? "Yes" : "No")
		<< ", Starved: " << (isStarved ? "Yes" : "No");
	return oss.str();
}
