#include "SimulationTimer.hpp"
#include <systemc.h>



SimulationTimer::SimulationTimer(double clock_period_sp, double reset_time, double warm_up_time, double total_time) :
	ClockPeriodPS(clock_period_sp), ResetTime(reset_time), WarmUpTime(warm_up_time), TotalTime(total_time) {
}
SimulationTimer::SimulationTimer(const SimulationTimer& timer) :
	ClockPeriodPS(timer.ClockPeriodPS), 
	ResetTime(timer.ResetTime),
	WarmUpTime(timer.WarmUpTime), 
	TotalTime(timer.TotalTime) {
}

double SimulationTimer::ClockPeriod() const {
	return ClockPeriodPS;
}

double SimulationTimer::SystemTime() const {
	return sc_time_stamp().to_double() / ClockPeriodPS;
}
double SimulationTimer::SimulationTime() const {
	return SystemTime() - ResetTime;
}
double SimulationTimer::StatisticsTime() const {
	return SimulationTime() - WarmUpTime;
}
double SimulationTimer::Progress() const {
	return SimulationTime() / TotalTime;
}
