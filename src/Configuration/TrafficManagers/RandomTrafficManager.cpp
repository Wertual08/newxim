#include "RandomTrafficManager.hpp"



RandomTrafficManager::RandomTrafficManager(std::uint32_t seed, std::int32_t count, double pir) :
	Random(seed), DestDistribution(0, count - 1), FireDistribution(0, 1),
	PacketInjectionRate(pir)
{
}

bool RandomTrafficManager::FirePacket(std::int32_t from, double time) const
{
	return FireDistribution(Random) < PacketInjectionRate;
}
std::int32_t RandomTrafficManager::FindDestination(std::int32_t from) const
{
	std::int32_t destination;
	while ((destination = DestDistribution(Random)) == from);
	return destination;
}