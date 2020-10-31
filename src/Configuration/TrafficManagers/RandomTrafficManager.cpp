#include "RandomTrafficManager.hpp"



RandomTrafficManager::RandomTrafficManager(std::uint32_t seed, std::int32_t count, double pir, double por) :
	Random(seed), DestDistribution(0, count - 1), FireDistribution(0, 1),
	PacketInjectionRate(pir), ProbabilityOfRetransmission(por)
{
}

bool RandomTrafficManager::FirePacket(std::int32_t from, double time, bool retransmitting) const
{
	return FireDistribution(Random) < (retransmitting ? ProbabilityOfRetransmission : PacketInjectionRate);
}
std::int32_t RandomTrafficManager::FindDestination(std::int32_t from) const
{
	std::int32_t destination;
	while ((destination = DestDistribution(Random)) == from);
	return destination;
}