#include "DiscScheduler.h"
#include <algorithm>
#include <queue>
#include <iostream>
#include <map>
#include <set>

static inline size_t absDiff(const size_t& a, const size_t& b) {
	return (a > b) ? a - b : b - a;
}

DiscScheduler::DiscScheduler(size_t disc_Size, size_t start_Head_Position, size_t time_Limit, size_t request_Count) 
	:disc_Size(disc_Size), start_Head_Position(start_Head_Position), time_Limit(time_Limit), request_Count(request_Count)
{}

DiscScheduler::~DiscScheduler() {}

std::vector<Request> DiscScheduler::fcfs(std::vector<Request>& requests) {
	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });

	std::queue<Request> queue;
	for (const Request& r : requests) {
		queue.push(r);
	}

	size_t pos = start_Head_Position;
	size_t time = 0;

	std::vector<Request> completed;

	completed.emplace_back(0, start_Head_Position, 0, 0, false); // dodanie pierwszej pozycji zeby wykres ladniej wygladal

	while (!queue.empty()) {
		Request curr = queue.front();
		if (curr.getArrivalTime() > time) {
			time = curr.getArrivalTime();
		}
		time += absDiff(pos, curr.getPosition());
		pos = curr.getPosition();
		curr.setWaitTime(time - curr.getArrivalTime());
		queue.pop();
		completed.push_back(curr);
	}
	return completed;
}

std::vector<Request> DiscScheduler::sstf(std::vector<Request>& requests) {
	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });
	size_t pos = start_Head_Position;
	size_t time = 0;

	std::vector<Request> vec;
	std::vector<Request> completed;

	completed.emplace_back(0, start_Head_Position, 0, 0, false);
	size_t count = 0;
	size_t i = 0;
	while (count < requests.size()) {
		while (i < requests.size() && requests[i].getArrivalTime() <= time) {
			vec.push_back(requests[i++]);
		}
		if (!vec.empty()) {
			auto target = std::min_element(vec.begin(), vec.end(), [&pos](const Request& a, const Request& b) {
				return absDiff(pos, a.getPosition()) < absDiff(pos, b.getPosition());
			});
			Request curr = *target;
			time += absDiff(pos, curr.getPosition());
			pos = curr.getPosition();
			curr.setWaitTime(time - curr.getArrivalTime());
			completed.push_back(curr);
			vec.erase(target);
			count++;
		} else {
			time++;
		}
	}
	return completed;
}

std::vector<Request> DiscScheduler::scan(std::vector<Request>& requests) {
	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });
	std::map<size_t, std::vector<Request>> disc;
	for (size_t i = 0; i <= disc_Size; i++) {
		std::pair<size_t, std::vector<Request>> p(i, {});
		disc.insert(p); // pusta mapa wszystkich miejsc na dysku
	}
	std::vector<Request> completed;
	completed.emplace_back(0, start_Head_Position, 0, 0, false);

	size_t time = 0;
	size_t i = 0;
	bool is_Going_Right = true;
	size_t current_Head_Position = start_Head_Position;
	size_t count = 0;
	size_t changes = 0;

	while (count < requests.size()) {
		while (i < requests.size() && requests[i].getArrivalTime() <= time) {
			disc[requests[i].getPosition()].push_back(requests[i]);
			i++;
		}
		if (!disc[current_Head_Position].empty()) {
			for (Request r : disc[current_Head_Position]) {
				r.setWaitTime(time - r.getArrivalTime());
				completed.push_back(r);
			}
			count += disc[current_Head_Position].size();
			disc[current_Head_Position].clear();
		}
		if (current_Head_Position >= disc_Size || current_Head_Position <= 0) {
			//is_Going_Right ? completed.emplace_back(0, disc_Size, time, 0, false) : completed.emplace_back(0, 0, time, 0, false);
			is_Going_Right = !is_Going_Right;
			changes++;// to tez jakos zanotowac
		}
		is_Going_Right ? current_Head_Position++ : current_Head_Position--;
		time++;
	}
	return completed;
}

std::vector<Request> DiscScheduler::cscan(std::vector<Request>& requests) {
	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });
	std::map<size_t, std::vector<Request>> disc;
	for (size_t i = 0; i <= disc_Size; i++) {
		std::pair<size_t, std::vector<Request>> p(i, {});
		disc.insert(p); // pusta mapa wszystkich miejsc na dysku
	}
	std::vector<Request> completed;
	completed.emplace_back(0, start_Head_Position, 0, 0, false);

	size_t time = 0;
	size_t i = 0;
	bool is_Going_Right = true;
	size_t current_Head_Position = start_Head_Position;
	size_t count = 0;
	size_t changes = 0;

	while (count < requests.size()) {
		while (i < requests.size() && requests[i].getArrivalTime() <= time) {
			disc[requests[i].getPosition()].push_back(requests[i]);
			i++;
		}
		if (!disc[current_Head_Position].empty()) {
			for (Request r : disc[current_Head_Position]) {
				r.setWaitTime(time - r.getArrivalTime());
				completed.push_back(r);
			}
			count += disc[current_Head_Position].size();
			disc[current_Head_Position].clear();
		}
		
		if (current_Head_Position >= disc_Size) {
			current_Head_Position = 0;
			//completed.emplace_back(0, disc_Size, time, 0, false);
			//completed.emplace_back(0, 0, time, 0, false);
		}else
			current_Head_Position++;
		time++;
	}
	return completed;
}

std::vector<Request> DiscScheduler::sstf_edf(std::vector<Request>& requests) {
	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {return a.getArrivalTime() < b.getArrivalTime(); });
	size_t pos = start_Head_Position;
	size_t time = 0;

	std::vector<Request> vec;
	std::vector<Request> rt_vec;
	std::vector<Request> completed;

	completed.emplace_back(0, start_Head_Position, 0, 0, false);
	size_t count = 0;
	size_t i = 0;
	while (count < requests.size()) {
		while (i < requests.size() && requests[i].getArrivalTime() <= time) {
			if (requests[i].getIsRealTime())
				rt_vec.push_back(requests[i++]);
			else
				vec.push_back(requests[i++]);
		}
		if (!rt_vec.empty()) {
			auto target = std::min_element(rt_vec.begin(), rt_vec.end(), [](const Request& a, const Request& b) {
				return a.getDeadline() < b.getDeadline();
				});
			Request curr = *target;
			time += absDiff(pos, curr.getPosition());
			pos = curr.getPosition();
			curr.setWaitTime(time - curr.getArrivalTime());
			completed.push_back(curr);
			rt_vec.erase(target);
			count++;
		}
		else if (!vec.empty()) {
			auto target = std::min_element(vec.begin(), vec.end(), [&pos](const Request& a, const Request& b) {
				return absDiff(pos, a.getPosition()) < absDiff(pos, b.getPosition());
				});
			Request curr = *target;
			time += absDiff(pos, curr.getPosition());
			pos = curr.getPosition();
			curr.setWaitTime(time - curr.getArrivalTime());
			completed.push_back(curr);
			vec.erase(target);
			count++;
		}
		else {
			time++;
		}
	}
	return completed;
}

std::vector<Request> DiscScheduler::sstf_fd_scan(std::vector<Request>& requests) {
	std::sort(requests.begin(), requests.end(), [](const Request& a, const Request& b) {
		return a.getArrivalTime() < b.getArrivalTime();
		});

	size_t pos = start_Head_Position;
	size_t time = 0;
	size_t count = 0;

	std::vector<Request> completed;
	std::map<size_t, std::vector<Request>> disc;

	// Initialize the disc map
	for (size_t i = 0; i <= disc_Size; i++) {
		disc[i] = {};
	}

	completed.emplace_back(0, start_Head_Position, 0, 0, false);

	std::vector<Request> rt_vec; // Real-time requests
	std::vector<Request> n_vec; // Non-real-time requests

	size_t i = 0;
	while (count < requests.size()) {
		// Add requests to the appropriate vectors based on arrival time
		while (i < requests.size() && requests[i].getArrivalTime() <= time) {
			if (requests[i].getIsRealTime()) {
				rt_vec.push_back(requests[i]);
			}
			else {
				n_vec.push_back(requests[i]);
			}
			disc[requests[i].getPosition()].push_back(requests[i]);
			i++;
		}

		// Process real-time requests first
		if (!rt_vec.empty()) {
			auto target = std::min_element(rt_vec.begin(), rt_vec.end(), [&pos, &time](const Request& a, const Request& b) {
				bool a_meets_deadline = (time + absDiff(pos, a.getPosition()) <= a.getArrivalTime() + a.getDeadline());
				bool b_meets_deadline = (time + absDiff(pos, b.getPosition()) <= b.getArrivalTime() + b.getDeadline());

				if (a_meets_deadline && b_meets_deadline) {
					return a.getDeadline() < b.getDeadline();
				}
				else if (a_meets_deadline) {
					return true;
				}
				else if (b_meets_deadline) {
					return false;
				}
				return false;
				});

			// Check if a valid target was found
			if (target != rt_vec.end() && time + absDiff(pos, (*target).getPosition()) <= (*target).getArrivalTime() + (*target).getDeadline()) {
				Request curr = *target;
				rt_vec.erase(target);

				// Move the head to the target position
				while (pos != curr.getPosition()) {
					if (!disc[pos].empty()) {
						for (Request& r : disc[pos]) {
							// Remove from rt_vec or n_vec if present
							auto rt_it = std::find(rt_vec.begin(), rt_vec.end(), r);
							if (rt_it != rt_vec.end()) {
								rt_vec.erase(rt_it);
							}
							else {
								auto n_it = std::find(n_vec.begin(), n_vec.end(), r);
								if (n_it != n_vec.end()) {
									n_vec.erase(n_it);
								}
							}
							r.setWaitTime(time - r.getArrivalTime());
							completed.push_back(r);
						}
						count += disc[pos].size();
						disc[pos].clear();
					}
					pos += (pos < curr.getPosition()) ? 1 : -1;
					time++;
				}

				// Process the target request
				for (Request& r : disc[pos]) {
					auto rt_it = std::find(rt_vec.begin(), rt_vec.end(), r);
					if (rt_it != rt_vec.end()) {
						rt_vec.erase(rt_it);
					}
					else {
						auto n_it = std::find(n_vec.begin(), n_vec.end(), r);
						if (n_it != n_vec.end()) {
							n_vec.erase(n_it);
						}
					}
					r.setWaitTime(time - r.getArrivalTime());
					completed.push_back(r);
				}
				count += disc[pos].size();
				disc[pos].clear();
			}
			else {
				count += rt_vec.size();
				// If no valid real-time request is found, fall back to non-real-time requests
				rt_vec.clear(); // Clear rt_vec since no valid real-time requests are left
			}
		}

		// Process non-real-time requests
		else if (!n_vec.empty()) {
			auto target = std::min_element(n_vec.begin(), n_vec.end(), [&pos](const Request& a, const Request& b) {
				return absDiff(pos, a.getPosition()) < absDiff(pos, b.getPosition());
				});

			Request curr = *target;
			n_vec.erase(target);

			// Move the head to the target position
			while (pos != curr.getPosition()) {
				if (!disc[pos].empty()) {
					for (Request& r : disc[pos]) {
						// Remove from rt_vec or n_vec if present
						auto rt_it = std::find(rt_vec.begin(), rt_vec.end(), r);
						if (rt_it != rt_vec.end()) {
							rt_vec.erase(rt_it);
						}
						else {
							auto n_it = std::find(n_vec.begin(), n_vec.end(), r);
							if (n_it != n_vec.end()) {
								n_vec.erase(n_it);
							}
						}
						r.setWaitTime(time - r.getArrivalTime());
						completed.push_back(r);
					}
					count += disc[pos].size();
					disc[pos].clear();
				}
				pos += (pos < curr.getPosition()) ? 1 : -1;
				time++;
			}

			// Process the target request
			for (Request& r : disc[pos]) {
				auto rt_it = std::find(rt_vec.begin(), rt_vec.end(), r);
				if (rt_it != rt_vec.end()) {
					rt_vec.erase(rt_it);
				}
				else {
					auto n_it = std::find(n_vec.begin(), n_vec.end(), r);
					if (n_it != n_vec.end()) {
						n_vec.erase(n_it);
					}
				}
				r.setWaitTime(time - r.getArrivalTime());
				completed.push_back(r);
			}
			count += disc[pos].size();
			disc[pos].clear();
		}

		// If no requests are available, increment time
		else {
			time++;
		}
	}

	return completed;
}

// znajdz czy istnieje zadanie rt
// jesli tak to idz do niego i usuwaj po kolei wszystko
// jesli nie to znajdz najblizsze zadanie pierwsze
// jesli istnieje to idz do niego
// jesli nie to zwieksz time