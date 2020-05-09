/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the top-level of Noxim
 */

#include "NoC.h"
#include "GlobalStats.h"
#include "DataStructs.h"
#include "Graph.h"
#include "ConfigurationManager.h"
#include "Configuration.h"
#include "ProgressBar.h"


// need to be globally visible to allow "-volume" simulation stop
unsigned int drained_volume;

int sc_main(int arg_num, char* arg_vet[])
{
	// TEMP
	drained_volume = 0;

	cout << "\t--------------------------------------------" << endl;
	cout << "\t\tNoxim - the NoC Simulator" << endl;
	cout << "\t\t(C) University of Catania" << endl;
	cout << "\t--------------------------------------------" << endl;

	cout << "Catania V., Mineo A., Monteleone S., Palesi M., and Patti D. (2016) Cycle-Accurate Network on Chip Simulation with Noxim. ACM Trans. Model. Comput. Simul. 27, 1, Article 4 (August 2016), 25 pages. DOI: https://doi.org/10.1145/2953878" << endl;
	cout << '\n' << '\n';

	configure(arg_num, arg_vet);

	Configuration Config(arg_num, arg_vet);
	NoC Network(Config);
	ProgressBar Bar(std::cout, Config.ResetTime(), Config.SimulationTime(), Config.ClockPeriodPS(), 20);
	Bar.clock(Network.clock);

	// Reset the chip
	Network.reset.write(1);
	cout << "Reset for " << Config.ResetTime() << " cycles... ";
	sc_start(Config.ResetTime(), SC_NS);
	Network.reset.write(0);
	cout << " done! " << endl;

	// Run the simulation
	cout << " Now running for " << GlobalParams::simulation_time << " cycles..." << endl;
	std::cout << " Progress: ";
	sc_start(GlobalParams::simulation_time, SC_NS);
	std::cout << '\n';
	
	// Close the simulation
	cout << "Noxim simulation completed.";
	cout << " (" << sc_time_stamp().to_double() / GlobalParams::clock_period_ps << " cycles executed)\n";
	cout << endl;

	// Show statistics
	GlobalStats gs(Network);
	gs.showStats(std::cout, GlobalParams::detailed);


	if ((GlobalParams::max_volume_to_be_drained > 0) && 
		(sc_time_stamp().to_double() / GlobalParams::clock_period_ps - 
			Config.ResetTime() >= GlobalParams::simulation_time))
	{
		cout << '\n'
			<< "WARNING! the number of flits specified with -volume option\n"
			<< "has not been reached. ( " << drained_volume << " instead of " << GlobalParams::max_volume_to_be_drained << " )\n"
			<< "You might want to try an higher value of simulation cycles\n"
			<< "using -sim option.\n";
	}

	return 0;
}
