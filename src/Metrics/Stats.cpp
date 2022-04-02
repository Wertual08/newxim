#include "Stats.hpp"



Stats::Stats(const SimulationTimer& timer) : Timer(timer), flits_routed(0)
{
}

void Stats::FlitRouted(const Flit& flit)
{
	flits_routed++;
}

void Stats::FlitReceived(std::int32_t relay, std::int32_t vc)
{
	auto& stats = Buffers[{ relay, vc }];
	stats.flits_recived++;
}

void Stats::StartStuckTimer(std::int32_t relay, std::int32_t vc)
{
	auto &stats = Buffers[{ relay, vc }];
	if (stats.stuck_timer >= 0) {
		return;
	}
	stats.stuck_timer = Timer.StatisticsTime();
}
void Stats::StopStuckTimer(std::int32_t relay, std::int32_t vc)
{
	auto& stats = Buffers[{ relay, vc }];
	if (stats.stuck_timer < 0) {
		return;
	}
	double delay = Timer.StatisticsTime() - stats.stuck_timer;
	if (delay > stats.max_suck_delay) {
		stats.max_suck_delay = delay;
	}
	stats.stuck_timer = -1;
}
void Stats::PushLoad(std::int32_t relay, std::int32_t vc, double load)
{
	auto& stats = Buffers[{ relay, vc }];
	stats.load_samples++;
	stats.total_load += load;
}

double Stats::GetMaxBufferStuckDelay(std::int32_t relay, std::int32_t vc)
{
	auto it = Buffers.find({ relay, vc });

	if (it != Buffers.end()) {
		return it->second.max_suck_delay;
	} else {
		return -1;
	}
}
std::int32_t Stats::GetBufferFlitsReceived(std::int32_t relay, std::int32_t vc)
{
	auto it = Buffers.find({ relay, vc });

	if (it != Buffers.end()) {
		return it->second.flits_recived;
	} else {
		return 0;
	}
}
double Stats::GetMaxBufferStuckDelay() 
{
	double result = -1;
	for (const auto& n : Buffers) {
		StopStuckTimer(n.first.port, n.first.vc);
		if (n.second.max_suck_delay > result) {
			result = n.second.max_suck_delay;
		}
	}
	return result;
}
double Stats::GetAverageBufferLoad(std::int32_t relay, std::int32_t vc) const
{
	auto it = Buffers.find({ relay, vc });

	if (it != Buffers.end()) {
		return it->second.total_load / it->second.load_samples;
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
			result += n.second.total_load / n.second.load_samples;
		}
		return result / Buffers.size();
	}
}
std::int32_t Stats::GetFlitsRouted() const
{
	return flits_routed;
}
