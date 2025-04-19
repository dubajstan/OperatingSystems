#pragma once

#include <random>
#include <memory>
#include "Request.h"
#include <vector>

enum Mode {
	NORMAL,
	UNIFORM,
};

class DistributionStrategy {
public:
	virtual size_t generate(std::mt19937_64& engine) = 0;
	virtual ~DistributionStrategy() = default;
};


class NormalDistribution : public DistributionStrategy {
private:
	double mean, stddev;
	std::normal_distribution<> dist;
	size_t min, max;
public:
	NormalDistribution(double mean, double stddev, size_t min, size_t max);

	size_t generate(std::mt19937_64& engine) override;
};


class UniformDistribution : public DistributionStrategy {
private:
	size_t min, max;
	std::uniform_int_distribution<size_t> dist;

public:
	UniformDistribution(size_t min, size_t max);

	size_t generate(std::mt19937_64& engine) override;
};


class RequestGenerator {
private:
	std::mt19937_64 engine;
	std::unique_ptr<DistributionStrategy> strategy_Position;
	std::unique_ptr<DistributionStrategy> strategy_Arrival_Time;

	void setStrategyPosition(std::unique_ptr<DistributionStrategy> new_Strategy);
	void setStrategyArrivalTime(std::unique_ptr<DistributionStrategy> new_Strategy);

public:
	RequestGenerator(size_t seed);
	RequestGenerator();
	~RequestGenerator();

	std::vector<Request> generateRequests(const size_t disc_Size, const size_t time_Limit, const size_t request_Count, const std::vector<std::pair<double, double>>& ranges, const Mode position_Mode, const Mode arrival_Time_Mode, bool isRealTime, double real_Time_Chance, const size_t d_Min, const size_t d_Max, const bool is_Anty_Scan);
	
};