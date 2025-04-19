#pragma once

#include <vector>
#include "Request.h"

class DiscScheduler {
private:
	size_t disc_Size, start_Head_Position, time_Limit, request_Count;

public:
	DiscScheduler(size_t disc_Size, size_t start_Head_Position, size_t time_Limit, size_t request_Count);
	~DiscScheduler();


	std::vector<Request> fcfs(std::vector<Request>& requests);
	std::vector<Request> sstf(std::vector<Request>& requests);
	std::vector<Request> scan(std::vector<Request>& requests);
	std::vector<Request> cscan(std::vector<Request>& requests);
	std::vector<Request> sstf_edf(std::vector<Request>& requests);
	std::vector<Request> sstf_fd_scan(std::vector<Request>& requests);
	

};