/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the top-level of Noxim
 */

#include "NoC.hpp"
#include "GlobalStats.hpp"
#include "DataStructs.hpp"
#include "Graph.hpp"
#include "ConfigurationManager.hpp"
#include "Configuration.hpp"
#include "ProgressBar.hpp"



int sc_main(int arg_num, char* arg_vet[])
{
	cout << "\t--------------------------------------------\n";
	cout << "\t\tNoxim - the NoC Simulator\n";
	cout << "\t\t(C) University of Catania\n";
	cout << "\t--------------------------------------------\n";

	cout << "Catania V., Mineo A., Monteleone S., Palesi M., and Patti D. (2016) Cycle-Accurate Network on Chip Simulation with Noxim. ACM Trans. Model. Comput. Simul. 27, 1, Article 4 (August 2016), 25 pages. DOI: https://doi.org/10.1145/2953878" << endl;
	cout << '\n' << '\n';

	Configuration Config(arg_num, arg_vet);
	if (Config.ReportRoutingTable())
	{
		std::cout << "Generated routing table: " << Config.GRTable() << '\n';
	}

	configure(arg_num, arg_vet);
	NoC Network(Config);
	std::unique_ptr<ProgressBar> Bar;
	if (Config.ReportProgress()) Bar = std::make_unique<ProgressBar>(std::cout, Config.ResetTime(), Config.SimulationTime(), Config.ClockPeriodPS(), 20, Network.clock);

	// Reset the chip
	Network.reset.write(1);
	cout << "Reset for " << Config.ResetTime() << " cycles... ";
	sc_start(Config.ResetTime(), SC_NS);
	Network.reset.write(0);
	cout << " done! " << endl;

	// Run the simulation
	cout << " Now running for " << GlobalParams::simulation_time << " cycles..." << endl;

	if (Config.ReportProgress())
	{
		std::cout << " Progress: ";
		sc_start(GlobalParams::simulation_time, SC_NS);
		std::cout << '\n';
	}
	else sc_start(GlobalParams::simulation_time, SC_NS);
	
	// Close the simulation
	cout << "Noxim simulation completed.";
	cout << " (" << static_cast<int32_t>(sc_time_stamp().to_double() / GlobalParams::clock_period_ps) << " cycles executed)\n";
	cout << '\n';

	// Show statistics
	std::cout << GlobalStats(Network, Config);
	return 0;
}
