#include "ProgressBar.h"



void ProgressBar::Update()
{
	int32_t stamp = sc_time_stamp().to_double() / ClockPeriod;
	if (stamp > TimeOffset)
	{
		int32_t level = BarUnits * (stamp - TimeOffset) / TotalTime;
		if (level > CurrentLevel)
		{
			if (CurrentLevel < 0)
			{
				Output << '[';
				for (int32_t i = 0; i < BarUnits; i++)
				{
					if (i <= level) Output << '#';
					else Output << ' ';
				}
				Output << ']';
			}
			else
			{
				for (int32_t i = 0; i <= BarUnits - CurrentLevel; i++) Output << '\b';
			
				for (int32_t i = CurrentLevel; i < BarUnits; i++)
				{
					if (i <= level) Output << '#';
					else Output << ' ';
				}
				Output << ']';
			}
			CurrentLevel = level;
		}
	}
}

ProgressBar::ProgressBar(sc_module_name, std::ostream& os, int32_t offset, int32_t total, int32_t period, int32_t units) :
	Output(os), TimeOffset(offset), TotalTime(total), ClockPeriod(period), BarUnits(units), CurrentLevel(-1)
{
}

ProgressBar::ProgressBar(std::ostream& os, int32_t offset, int32_t total, int32_t period, int32_t units) :
	ProgressBar("ProgressBar", os, offset, total, period, units)
{
	SC_METHOD(Update);
	sensitive << clock.pos();
}
