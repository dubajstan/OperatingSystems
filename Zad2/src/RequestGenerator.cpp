#include "RequestGenerator.h"
#include <stdexcept>
#include <iostream>
#include <cmath> // dla std::round

RequestGenerator::RequestGenerator(size_t seed)
	: engine(seed)
{
}

RequestGenerator::RequestGenerator() {
	std::random_device rd;
	engine.seed(rd());
	strategy_Position = nullptr;
	strategy_Arrival_Time = nullptr;
}

RequestGenerator::~RequestGenerator() {}

NormalDistribution::NormalDistribution(double mean, double stddev, size_t min, size_t max)
	: mean(mean), stddev(stddev), dist(mean, stddev), min(min), max(max)
{
}

size_t NormalDistribution::generate(std::mt19937_64& engine) {
	double result;
	int attempts = 0;
	const int max_attempts = 100;

	do {
		result = dist(engine);
		attempts++;
		if (attempts > max_attempts)
			throw std::runtime_error("Exceeded max attempts in NormalDistribution::generate()");
	} while (result < static_cast<double>(min) || result > static_cast<double>(max));

	return static_cast<size_t>(std::round(result));
}

UniformDistribution::UniformDistribution(size_t min, size_t max)
	: min(min), max(max), dist(min, max)
{
}

size_t UniformDistribution::generate(std::mt19937_64& engine) {
	return dist(engine); // ju¿ masz ustawiony przedzia³ w konstruktorze
}

void RequestGenerator::setStrategyPosition(std::unique_ptr<DistributionStrategy> new_Strategy) {
	strategy_Position = std::move(new_Strategy);
}

void RequestGenerator::setStrategyArrivalTime(std::unique_ptr<DistributionStrategy> new_Strategy) {
	strategy_Arrival_Time = std::move(new_Strategy);
}

std::vector<Request> RequestGenerator::generateRequests(
	const size_t disc_Size,
	const size_t time_Limit,
	const size_t request_Count,
	const std::vector<std::pair<double, double>>& ranges,
	const Mode position_Mode,
	const Mode arrival_Time_Mode,
	bool isRealTime,
	double real_Time_Chance,
	const size_t d_Min,
	const size_t d_Max,
	const bool is_Anty_Scan
) {
	if (real_Time_Chance < 0.0 || real_Time_Chance > 1.0)
		throw std::runtime_error("Incorrect chance argument, should be a double between 0.0 and 1.0");

	std::bernoulli_distribution dist(real_Time_Chance);
	std::vector<Request> requests;
	size_t id = 1;
	size_t total_requests = 0;

	for (const auto& p : ranges) {
		if (p.first < 0.0 || p.first > 1.0 || p.second < 0.0 || p.second > 1.0 || p.first >= p.second)
			throw std::runtime_error("Incorrect range arguments");

		size_t min_Pos = static_cast<size_t>(std::round(p.first * disc_Size));
		size_t max_Pos = static_cast<size_t>(std::round(p.second * disc_Size));
		size_t range_Count = request_Count / ranges.size();

		// zabezpieczenie przed pustym zakresem
		if (min_Pos >= max_Pos) max_Pos = min_Pos + 1;

		switch (position_Mode) {
		case Mode::NORMAL:
			setStrategyPosition(std::make_unique<NormalDistribution>(
				(min_Pos + max_Pos) / 2.0,
				(max_Pos - min_Pos) / 4.0,
				min_Pos,
				max_Pos
			));
			break;
		case Mode::UNIFORM:
			setStrategyPosition(std::make_unique<UniformDistribution>(min_Pos, max_Pos));
			break;
		default:
			throw std::runtime_error("Unrecognized position mode");
		}

		switch (arrival_Time_Mode) {
		case Mode::NORMAL:
			setStrategyArrivalTime(std::make_unique<NormalDistribution>(
				time_Limit / 2.0,
				time_Limit / 3.0,
				0,
				time_Limit
			));
			break;
		case Mode::UNIFORM:
			setStrategyArrivalTime(std::make_unique<UniformDistribution>(0, time_Limit));
			break;
		default:
			throw std::runtime_error("Unrecognized arrival time mode");
		}

		if (is_Anty_Scan) {
			size_t i = 50;
			size_t c = 0;
			bool going_Right = true;
			while (c < range_Count && total_requests < request_Count) {
				bool rt = isRealTime && dist(engine);
				size_t deadline = rt ? std::uniform_int_distribution<size_t>(d_Min, d_Max)(engine) : 0;

				Request r(id++, i, strategy_Arrival_Time->generate(engine), deadline, rt);
				requests.push_back(r);
				total_requests++;
				c++;
				if (going_Right) i++;
				else i--;
				if (i <= 0 || i >= disc_Size) {
					going_Right = !going_Right;
				}
			}
		}
		else {
			for (size_t i = 0; i < range_Count && total_requests < request_Count; i++) {
				bool rt = isRealTime && dist(engine);
				size_t deadline = rt ? std::uniform_int_distribution<size_t>(d_Min, d_Max)(engine) : 0;

				Request r(id++, strategy_Position->generate(engine), strategy_Arrival_Time->generate(engine), deadline, rt);
				requests.push_back(r);
				total_requests++;
			}
		}
	}

	return requests;
}
