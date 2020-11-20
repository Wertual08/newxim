/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the top-level of Noxim
 */

#include "Hardware/SimulationTimer.hpp"
#include "Hardware/NoC.hpp"
#include "Metrics/GlobalStats.hpp"
#include "DataStructs.hpp"
#include "Configuration/Graph.hpp"
#include "Configuration/Configuration.hpp"
#include "Metrics/ProgressBar.hpp"

const static std::string Version = "0.0.0.5";



int sc_main(int arg_num, char* arg_vet[])
{
	std::cout << "    --------------------------------------------------------\n";
	std::cout << "        Newxim - modification of the Noxim NoC Simulator\n";
	std::cout << "        (C) Higher School of Economics University\n";
	std::cout << "        Version: " << Version << '\n';
	std::cout << "    --------------------------------------------------------\n";

	std::cout << "\n\n";

	Configuration Config(arg_num, arg_vet);
	SimulationTimer Timer(Config.ClockPeriodPS(), Config.ResetTime(), Config.StatsWarmUpTime(), Config.SimulationTime());
	NoC Network(Config, Timer);
	GlobalStats stats(Network, Config);

	std::unique_ptr<ProgressBar> Bar;
	if (Config.ReportProgress()) Bar = std::make_unique<ProgressBar>(std::cout, Timer, 20, Network.clock);

	// Reset the chip
	Network.reset.write(true);
	std::cout << "Reset for " << Config.ResetTime() << " cycles... ";
	sc_start(Config.ResetTime(), SC_NS);
	Network.reset.write(false);
	std::cout << " done!\n";

	// Run the simulation
	std::cout << " Now running for " << Config.SimulationTime() << " cycles...\n";

	if (Config.ReportProgress()) std::cout << " Progress: ";
	sc_start(Config.SimulationTime(), SC_NS);
	if (Config.ReportProgress()) std::cout << '\n';
	
	// Close the simulation
	std::cout << "Noxim simulation completed.";
	std::cout << " (" << static_cast<std::int32_t>(Timer.SystemTime()) << " cycles executed)\n";
	std::cout << '\n';

	// Show statistics
	std::cout << stats;
	return 0;
}
