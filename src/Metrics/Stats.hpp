#pragma once
#include <map>
#include "Hardware/SimulationTimer.hpp"



struct RelaySignature
{
	std::int32_t relay_id;
	std::int32_t vc_id;

	bool operator<(const RelaySignature& sig) const
	{
		if (relay_id < sig.relay_id) return true;
		if (relay_id > sig.relay_id) return false;
		if (vc_id < sig.vc_id) return true;
		return false;
	}
};
struct BufferStats
{
	double StuckTimer = -1;
	double MaxStuckDelay = 0;
	double TotalLoad = 0;
	double LoadSamples = 0;
};

class Stats 
{
private:
	std::map<RelaySignature, BufferStats> Buffers;
	std::int32_t flits_routed;

public:
	const SimulationTimer Timer;
	Stats(const SimulationTimer& timer);

	void FlitRouted();

	void StartStuckTimer(std::int32_t relay, std::int32_t vc);
	void StopStuckTimer(std::int32_t relay, std::int32_t vc);
	void PushLoad(std::int32_t relay, std::int32_t vc, double load);

	double GetMaxBufferStuckDelay(std::int32_t relay, std::int32_t vc);
	double GetMaxBufferStuckDelay();
	double GetAverageBufferLoad(std::int32_t relay, std::int32_t vc) const;
	double GetAverageBufferLoad() const;
	std::int32_t GetFlitsRouted() const;
};
