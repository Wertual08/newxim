/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the top-level of Noxim
 */

#include "SimulationTimer.hpp"
#include "NoC.hpp"
#include "GlobalStats.hpp"
#include "DataStructs.hpp"
#include "Graph.hpp"
#include "Configuration.hpp"
#include "ProgressBar.hpp"

const static std::string Version = "0.0.0.0";



int sc_main(int arg_num, char* arg_vet[])
{
	std::cout << "    --------------------------------------------------------\n";
	std::cout << "        Newxim - modification of the Noxim NoC Simulator\n";
	std::cout << "        (C) Higher School of Economics University\n";
	std::cout << "        Version: " << Version << '\n';
	std::cout << "    --------------------------------------------------------\n";

	//cout << "Catania V., Mineo A., Monteleone S., Palesi M., and Patti D. (2016) Cycle-Accurate Network on Chip Simulation with Noxim. ACM Trans. Model. Comput. Simul. 27, 1, Article 4 (August 2016), 25 pages. DOI: https://doi.org/10.1145/2953878" << endl;
	std::cout << "\n\n";

	Configuration Config(arg_num, arg_vet);
	if (Config.ReportRoutingTable())
	{
		std::cout << "Generated routing table: " << Config.GRTable() << '\n';
	}

	SimulationTimer Timer(Config.ClockPeriodPS(), Config.ResetTime(), Config.StatsWarmUpTime(), Config.SimulationTime());
	NoC Network(Config, Timer);
	std::unique_ptr<ProgressBar> Bar;
	if (Config.ReportProgress()) Bar = std::make_unique<ProgressBar>(std::cout, Timer, 20, Network.clock);

	// Reset the chip
	Network.reset.write(1);
	std::cout << "Reset for " << Config.ResetTime() << " cycles... ";
	sc_start(Config.ResetTime(), SC_NS);
	Network.reset.write(0);
	std::cout << " done! " << endl;

	// Run the simulation
	std::cout << " Now running for " << Config.SimulationTime() << " cycles..." << endl;

	if (Config.ReportProgress())
	{
		std::cout << " Progress: ";
		sc_start(Config.SimulationTime(), SC_NS);
		std::cout << '\n';
	}
	else sc_start(Config.SimulationTime(), SC_NS);
	
	// Close the simulation
	std::cout << "Noxim simulation completed.";
	std::cout << " (" << static_cast<int32_t>(Timer.SystemTime()) << " cycles executed)\n";
	std::cout << '\n';

	// Show statistics
	std::cout << GlobalStats(Network, Config);
	return 0;
}
