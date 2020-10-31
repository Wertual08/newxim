#include "ProgressBar.hpp"



void ProgressBar::Update()
{
	if (Timer.SimulationTime() >= 0.0)
	{
		std::int32_t level = BarUnits * Timer.Progress();
		std::int32_t percent = std::round(100.0 * Timer.Progress());
		if (level > CurrentLevel || percent > CurrentPercent)
		{
			if (CurrentLevel < 0)
			{
				std::string percent_str = std::to_string(percent) + '%';
				Output << '[';
				for (std::int32_t i = 0; i < BarUnits; i++)
				{
					if (i == BarUnits / 2 - percent_str.size() / 2)
					{
						Output << percent_str;
						i += percent_str.size() - 1;
					}
					else
					{
						if (i <= level) Output << '#';
						else Output << ' ';
					}
				}
				Output << ']';
			}
			else
			{
				std::string percent_str = std::to_string(percent) + '%';
				std::int32_t start = std::min(CurrentLevel, BarUnits / 2 - static_cast<std::int32_t>(percent_str.size()) / 2);
				for (std::int32_t i = start; i <= BarUnits; i++) Output << '\b';
			
				for (std::int32_t i = start; i < BarUnits; i++)
				{
					if (i == BarUnits / 2 - percent_str.size() / 2)
					{
						Output << percent_str;
						i += percent_str.size() - 1;
					}
					else
					{
						if (i <= level) Output << '#';
						else Output << ' ';
					}
				}
				Output << ']';
			}
			CurrentLevel = level;
			CurrentPercent = percent;
		}
	}
}

ProgressBar::ProgressBar(sc_module_name, std::ostream& os, const SimulationTimer& timer, std::int32_t units, const sc_clock& clk) :
	Output(os), Timer(timer), BarUnits(units), CurrentLevel(-1), CurrentPercent(-1)
{
	clock(clk);
}

ProgressBar::ProgressBar(std::ostream& os, const SimulationTimer& timer, std::int32_t units, const sc_clock& clk) :
	ProgressBar("ProgressBar", os, timer, units, clk)
{
	SC_METHOD(Update);
	sensitive << clock.pos();
}
