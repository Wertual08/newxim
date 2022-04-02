#pragma once
#include <map>
#include "Hardware/SimulationTimer.hpp"
#include "Data/Flit.hpp"
#include "Hardware/Connection.hpp"




struct BufferStats
{
	double stuck_timer = -1;
	double max_suck_delay = -1;
	double total_load = 0;
	double load_samples = 0;
	std::int32_t flits_recived = 0;
};

class Stats 
{
private:
	std::map<Connection, BufferStats> Buffers;
	std::int32_t flits_routed;

public:
	const SimulationTimer Timer;
	Stats(const SimulationTimer& timer);

	void FlitRouted(const Flit& flit);
	void FlitReceived(std::int32_t relay, std::int32_t vc);

	void StartStuckTimer(std::int32_t relay, std::int32_t vc);
	void StopStuckTimer(std::int32_t relay, std::int32_t vc);
	void PushLoad(std::int32_t relay, std::int32_t vc, double load);

	double GetMaxBufferStuckDelay(std::int32_t relay, std::int32_t vc);
	std::int32_t GetBufferFlitsReceived(std::int32_t relay, std::int32_t vc);
	double GetMaxBufferStuckDelay();
	double GetAverageBufferLoad(std::int32_t relay, std::int32_t vc) const;
	double GetAverageBufferLoad() const;
	std::int32_t GetFlitsRouted() const;
};
