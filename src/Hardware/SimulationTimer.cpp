#include "SimulationTimer.hpp"
#include <systemc.h>



SimulationTimer::SimulationTimer(
	double clock_period_ps_, 
	double reset_time_, 
	double warm_up_time_, 
	double total_time_,
	double production_time_
) :
	clock_period_ps(clock_period_ps_),
	reset_time(reset_time_),
	warm_up_time(warm_up_time_),
	total_time(total_time_),
	production_time(production_time_) {
}
SimulationTimer::SimulationTimer(const SimulationTimer& timer) :
	clock_period_ps(timer.clock_period_ps), 
	reset_time(timer.reset_time),
	warm_up_time(timer.warm_up_time), 
	total_time(timer.total_time),
	production_time(timer.production_time) {
}

double SimulationTimer::ClockPeriod() const {
	return clock_period_ps;
}

double SimulationTimer::SystemTime() const {
	return sc_time_stamp().to_double() / clock_period_ps;
}
double SimulationTimer::SimulationTime() const {
	return SystemTime() - reset_time;
}
double SimulationTimer::StatisticsTime() const {
	return SimulationTime() - warm_up_time;
}
double SimulationTimer::ProductionTime() const {
	return production_time;
}
double SimulationTimer::Progress() const {
	return SimulationTime() / total_time;
}
