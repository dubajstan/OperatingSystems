#pragma once

#include <random>
#include <algorithm>
#include <vector>
#include <memory>
#include <stdexcept>

#include "Process.h"

enum Mode {
	GAUSSIAN,
	LINEAR,
	INVERSE_LINEAR
};

class DistributionStrategy {
public:
	virtual size_t generate(double min, double max, std::mt19937& engine) = 0;
	virtual ~DistributionStrategy() = default;
};

class GaussianDistribution : public DistributionStrategy {
public:
	size_t generate(double min, double max, std::mt19937& engine) override {
		double mean = 0.5;
		double stddev = 0.2;
		std::normal_distribution<> dist(mean, stddev);

		double normalized = std::clamp(dist(engine), 0.0, 1.0);
		double val = normalized * (max - min) + min;

		if (val >= std::numeric_limits<size_t>::max()) {
			throw "Wartosc generowana przed rozklad gaussa jest zbyt duza";
		}
		else {
			return static_cast<size_t>(std::round(val));
		}
	}
};

class LinearDistribution : public DistributionStrategy {
public:
	size_t generate(double min, double max, std::mt19937& engine) override {
		std::piecewise_linear_distribution<> dist({ 0.0, 0.5, 1.0 }, [](double x) { return x; });
		double normalized = std::clamp(dist(engine), 0.0, 1.0);
		double val = normalized * (max - min) + min;

		if (val >= std::numeric_limits<size_t>::max()) {
			throw "Wartosc generowana przed rozklad gaussa jest zbyt duza";
		}
		else {
			return static_cast<size_t>(std::round(val));
		}
	}
};

class InverseLinearDistribution : public DistributionStrategy {
public:
	size_t generate(double min, double max, std::mt19937& engine) override {
		std::piecewise_linear_distribution<> dist({ 0.0, 0.5, 1.0 }, [](double x) { return 1 - x; });
		double normalized = std::clamp(dist(engine), 0.0, 1.0);
		double val = normalized * (max - min) + min;

		if (val >= std::numeric_limits<size_t>::max()) {
			throw "Wartosc generowana przed rozklad gaussa jest zbyt duza";
		}
		else {
			return static_cast<size_t>(std::round(val));
		}
	}
};


class ProcessGenerator {
private:
	std::mt19937 engine;

	std::unique_ptr<DistributionStrategy> arrivalTimeStrategy;
	std::unique_ptr<DistributionStrategy> burstTimeStrategy;

	size_t generateDiscreteDistribution(const std::vector<size_t>& priorities) {
		std::discrete_distribution<size_t> dist(priorities.begin(), priorities.end());
		return dist(engine) + 1;
	}

	inline void setArrivalTimeStrategy(std::unique_ptr<DistributionStrategy> strategy) {
		this->arrivalTimeStrategy = std::move(strategy);
	}

	inline void setBurstTimeStrategy(std::unique_ptr<DistributionStrategy> strategy) {
		this->burstTimeStrategy = std::move(strategy);
	}

public:
	explicit ProcessGenerator() {
		std::random_device rd;
		engine.seed(rd());
	}

	explicit ProcessGenerator(unsigned int seed) {
		engine.seed(seed);
	}

	~ProcessGenerator() {}

	template<typename T>
	T getRandomElement(const std::vector<T>& elements) {
		if (elements.empty()) {
			throw std::runtime_error("Pusta lista elementow");
		}
		size_t randomIndex = rand() % elements.size();
		return elements[randomIndex];
	}

	std::vector<Process> generateProcesses(size_t count, Mode ModeArrivalTime, double minArrivalTime, double maxArrivalTime, Mode ModeBurstTime, double minBurstTime, double maxBurstTime, std::vector<size_t> priorities) {
		std::vector<Process> processes;
		processes.reserve(count);

		switch (ModeArrivalTime) {
		case GAUSSIAN:
			setArrivalTimeStrategy(std::make_unique<GaussianDistribution>());
			break;
		case LINEAR:
			setArrivalTimeStrategy(std::make_unique<LinearDistribution>());
			break;
		case INVERSE_LINEAR:
			setArrivalTimeStrategy(std::make_unique<InverseLinearDistribution>());
			break;
		default:
			throw std::runtime_error("Nieznana strategia Arrival Time");
			break;
		}
		switch (ModeBurstTime) {
			{
		case GAUSSIAN:
			setBurstTimeStrategy(std::make_unique<GaussianDistribution>());
			break;
		case LINEAR:
			setBurstTimeStrategy(std::make_unique<LinearDistribution>());
			break;
		case INVERSE_LINEAR:
			setBurstTimeStrategy(std::make_unique<InverseLinearDistribution>());
			break;
		default:
			throw std::runtime_error("Nieznana strategia Burst Time");
			break;
			}
		}
		if (!arrivalTimeStrategy || !burstTimeStrategy) {
			throw std::runtime_error("Strategia nie zostala ustawiona");
		}

		for (size_t i = 0; i < count; i++) {
			processes.emplace_back(i + 1, generateDiscreteDistribution(priorities), arrivalTimeStrategy->generate(minArrivalTime, maxArrivalTime, engine), burstTimeStrategy->generate(minBurstTime, maxBurstTime, engine));
		}

		std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
			return a.getArrivalTime() < b.getArrivalTime();
			}
		);

		return processes;
	}
};