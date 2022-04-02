#pragma once



class SimulationTimer
{
private:
	const double clock_period_ps;
	const double reset_time;
	const double warm_up_time;
	const double total_time;
	const double production_time;

public:
	SimulationTimer(
		double clock_period_sp, 
		double reset_time, 
		double warm_up_time, 
		double total_time, 
		double production_time
	);
	SimulationTimer(const SimulationTimer& timer);

	double ClockPeriod() const;

	double SystemTime() const;
	double SimulationTime() const;
	double StatisticsTime() const;
	double ProductionTime() const;
	double Progress() const;
};