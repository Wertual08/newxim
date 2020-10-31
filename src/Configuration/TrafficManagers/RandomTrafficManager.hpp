#pragma once
#include "TrafficManager.hpp"
#include <random>



class RandomTrafficManager : public TrafficManager
{
private:
	mutable std::default_random_engine Random;
	std::uniform_int_distribution<std::int32_t> DestDistribution;
	std::uniform_real_distribution<double> FireDistribution;
	const double PacketInjectionRate;
	const double ProbabilityOfRetransmission;

public:
	RandomTrafficManager(std::uint32_t seed, std::int32_t count, double pir, double por);

	virtual bool FirePacket(std::int32_t from, double time, bool retransmitting) const override;
	virtual std::int32_t FindDestination(std::int32_t from) const override;
};