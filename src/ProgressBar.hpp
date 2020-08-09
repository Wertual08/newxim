#pragma once
#include <systemc.h>
#include "SimulationTimer.hpp"



class ProgressBar : public sc_module
{
	SC_HAS_PROCESS(ProgressBar);
private:
	const SimulationTimer Timer;
	const int32_t BarUnits;
	int32_t CurrentLevel, CurrentPercent;
	std::ostream& Output;
	sc_in_clk clock;

	void Update();
	ProgressBar(sc_module_name, std::ostream& os, const SimulationTimer& network_timer, int32_t units, const sc_clock& clk);

public:

	ProgressBar(std::ostream& os, const SimulationTimer& network_timer, int32_t units, const sc_clock& clk);
};