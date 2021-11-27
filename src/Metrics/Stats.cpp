#include "Stats.hpp"



Stats::Stats(const SimulationTimer& timer) : Timer(timer), flits_routed(0)
{
}

void Stats::FlitRouted()
{
	flits_routed++;
}

void Stats::StartStuckTimer(std::int32_t relay, std::int32_t vc)
{
	auto &stats = Buffers[{ relay, vc }];
	if (stats.StuckTimer >= 0) {
		return;
	}
	stats.StuckTimer = Timer.StatisticsTime();
}
void Stats::StopStuckTimer(std::int32_t relay, std::int32_t vc)
{
	auto& stats = Buffers[{ relay, vc }];
	if (stats.StuckTimer < 0) {
		return;
	}
	double delay = Timer.StatisticsTime() - stats.StuckTimer;
	if (delay > stats.MaxStuckDelay) {
		stats.MaxStuckDelay = delay;
	}
	stats.StuckTimer = -1;
}
void Stats::PushLoad(std::int32_t relay, std::int32_t vc, double load)
{
	auto& stats = Buffers[{ relay, vc }];
	stats.LoadSamples++;
	stats.TotalLoad += load;
}

double Stats::GetMaxBufferStuckDelay(std::int32_t relay, std::int32_t vc)
{
	auto it = Buffers.find({ relay, vc });

	if (it != Buffers.end()) {
		return it->second.MaxStuckDelay;
	} else {
		return -1;
	}
}
double Stats::GetMaxBufferStuckDelay() 
{
	double result = -1;
	for (const auto& n : Buffers) {
		StopStuckTimer(n.first.relay_id, n.first.vc_id);
		if (n.second.MaxStuckDelay > result) {
			result = n.second.MaxStuckDelay;
		}
	}
	return result;
}
double Stats::GetAverageBufferLoad(std::int32_t relay, std::int32_t vc) const
{
	auto it = Buffers.find({ relay, vc });

	if (it != Buffers.end()) {
		return it->second.TotalLoad / it->second.LoadSamples;
	} else {
		return -1;
	}
}
double Stats::GetAverageBufferLoad() const
{
	if (Buffers.empty()) {
		return -1;
	} else {
		double result = 0;
		for (const auto& n : Buffers) {
			result += n.second.TotalLoad / n.second.LoadSamples;
		}
		return result / Buffers.size();
	}
}
std::int32_t Stats::GetFlitsRouted() const
{
	return flits_routed;
}
