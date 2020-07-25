#include "TrafficManager.hpp"



int32_t randInt(int32_t min, int32_t max)
{
	return min + (int32_t)((double)(max - min + 1) * rand() / (RAND_MAX + 1.0));
}

void TrafficManager::Init(int32_t count)
{
	TrafficLoad.resize(count);
}
void TrafficManager::SetLoad(int32_t node, int32_t send_factor, int32_t receive_factor)
{
	TrafficLoad[node] = { send_factor, receive_factor };
}
void TrafficManager::Setup()
{
	Destinations.clear();
	for (int32_t i = 0; i < TrafficLoad.size(); i++)
		for (int32_t j = 0; j < TrafficLoad[i].ReceiveFactor; j++)
			Destinations.push_back(i);
}

bool TrafficManager::FirePacket(int32_t from, double threshold) const
{
	return (double)rand() / RAND_MAX * TrafficLoad[from].SendFactor < threshold;
}
int32_t TrafficManager::FindDestination(int32_t from) const
{
	int32_t destination;
	while ((destination = Destinations[randInt(0, Destinations.size() - 1)]) == from);
	return destination;
}
