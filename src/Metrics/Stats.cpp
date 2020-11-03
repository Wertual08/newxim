#include "Stats.hpp"



Stats::Stats(const SimulationTimer& timer) : Timer(timer)
{
}

void Stats::StartStuckTimer(std::size_t relay, std::size_t vc)
{
	Buffers[{ relay, vc }].StuckTimer = Timer.StatisticsTime();
}
void Stats::StopStuckTimer(std::size_t relay, std::size_t vc)
{
	auto& stats = Buffers[{ relay, vc }];
	if (stats.StuckTimer < 0) return;
	double delay = Timer.StatisticsTime() - stats.StuckTimer;
	if (delay > stats.MaxStuckDelay) stats.MaxStuckDelay = delay;
	stats.StuckTimer = -1;
}
void Stats::PushLoad(std::size_t relay, std::size_t vc, double load)
{
	auto& stats = Buffers[{ relay, vc }];
	stats.LoadSamples++;
	stats.TotalLoad += load;
}

double Stats::GetMaxBufferStuckDelay(std::size_t relay, std::size_t vc) const
{
	auto it = Buffers.find({ relay, vc });

	if (it != Buffers.end()) return it->second.MaxStuckDelay;
	else return -1;
}
double Stats::GetMaxBufferStuckDelay() const
{
	double result = -1;
	for (const auto& n : Buffers)
		if (n.second.MaxStuckDelay > result)
			result = n.second.MaxStuckDelay;
	return result;
}
double Stats::GetAverageBufferLoad(std::size_t relay, std::size_t vc) const
{
	auto it = Buffers.find({ relay, vc });

	if (it != Buffers.end()) return it->second.TotalLoad / it->second.LoadSamples;
	else return -1;
}
double Stats::GetAverageBufferLoad() const
{
	if (Buffers.empty()) return -1;
	else
	{
		double result = 0;
		for (const auto& n : Buffers)
			result += n.second.TotalLoad / n.second.LoadSamples;
		return result / Buffers.size();
	}
}
