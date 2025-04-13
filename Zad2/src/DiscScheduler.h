#pragma once

#include <vector>
#include "Request.h"

class DiscScheduler {
private:
	size_t disc_Size, start_Head_Position, time_Limit, request_Count;
	std::vector<Request> requests, finished_Requests, missed_Requests;

public:
	DiscScheduler(size_t disc_Size, size_t start_Head_Position, size_t time_Limit, size_t request_Count);
	~DiscScheduler();

	

};