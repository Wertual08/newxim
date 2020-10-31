#pragma once



class SimulationTimer
{
private:
	const double ClockPeriodPS;
	const double ResetTime;
	const double WarmUpTime;
	const double TotalTime;

public:
	SimulationTimer(double clock_period_sp, double reset_time, double warm_up_time, double total_time);
	SimulationTimer(const SimulationTimer& timer);

	double ClockPeriod() const;

	double SystemTime() const;
	double SimulationTime() const;
	double StatisticsTime() const;
	double Progress() const;
};