#pragma once
#include <systemc.h>
#include "Hardware/SimulationTimer.hpp"



class ProgressBar : public sc_module
{
	SC_HAS_PROCESS(ProgressBar);
private:
	const SimulationTimer Timer;
	const std::int32_t BarUnits;
	std::int32_t CurrentLevel, CurrentPercent;
	std::ostream& Output;
	sc_in_clk clock;

	void Update();
	ProgressBar(sc_module_name, std::ostream& os, const SimulationTimer& network_timer, std::int32_t units, const sc_clock& clk);

public:

	ProgressBar(std::ostream& os, const SimulationTimer& network_timer, std::int32_t units, const sc_clock& clk);
};