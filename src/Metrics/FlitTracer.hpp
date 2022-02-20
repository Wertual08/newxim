#pragma once
#include <cstdint>
#include <vector>
#include "Data/Flit.hpp"
#include "Hardware/SimulationTimer.hpp"



class FlitTracer
{
public:
	struct Location
	{
		std::int32_t id;
		std::int32_t port;
		std::int32_t vc;
		double timestamp;
	};

private:
	SimulationTimer Timer;
	double StoryStart, StoryEnd;
	std::uint64_t IDOffset = 0;
	std::vector<std::pair<Flit, std::vector<Location>>> FlitHistory;

public:
	FlitTracer(const SimulationTimer& timer, double story_start, double story_end);

	void Register(Flit& flit);
	void Remember(const Flit& flit, std::int32_t id);

	friend std::ostream& operator<<(std::ostream& os, const FlitTracer& tracer);
};