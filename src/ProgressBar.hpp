#pragma once
#include <systemc.h>



class ProgressBar : public sc_module
{
	SC_HAS_PROCESS(ProgressBar);
private:
	const int32_t TimeOffset, TotalTime, ClockPeriod, BarUnits;
	int32_t CurrentLevel = -1;
	std::ostream& Output;

	void Update();
	ProgressBar(sc_module_name, std::ostream& os, int32_t offset, int32_t total, int32_t period, int32_t units);

public:
	sc_in_clk clock;

	ProgressBar(std::ostream& os, int32_t offset, int32_t total, int32_t period, int32_t units);
};