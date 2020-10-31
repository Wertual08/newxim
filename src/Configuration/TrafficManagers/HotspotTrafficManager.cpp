#include "HotspotTrafficManager.hpp"



HotspotTrafficManager::HotspotTrafficManager(std::uint32_t seed, std::int32_t count, double pir, double por,
	const std::vector<std::pair<std::int32_t, std::pair<std::int32_t, std::int32_t>>>& hotspots) :
	Random(seed), PacketInjectionRate(pir), ProbabilityOfRetransmission(por),
	TrafficLoad(count, std::make_pair(1, 1))
{
	for (const auto& hotspot : hotspots) TrafficLoad[hotspot.first] = hotspot.second;

	for (std::int32_t i = 0; i < TrafficLoad.size(); i++)
		for (std::int32_t j = 0; j < TrafficLoad[i].second; j++)
			Destinations.push_back(i);
	DestDistribution = std::uniform_int_distribution<std::int32_t>(0, Destinations.size() - 1);
	FireDistribution = std::uniform_real_distribution<double>(0, 1);
}

bool HotspotTrafficManager::FirePacket(std::int32_t from, double time, bool retransmitting) const
{
	return FireDistribution(Random) * TrafficLoad[from].first < (retransmitting ? ProbabilityOfRetransmission : PacketInjectionRate);
}
std::int32_t HotspotTrafficManager::FindDestination(std::int32_t from) const
{
	std::int32_t destination;
	while ((destination = Destinations[DestDistribution(Random)]) == from);
	return destination;
}