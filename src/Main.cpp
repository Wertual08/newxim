#include "Hardware/SimulationTimer.hpp"
#include "Hardware/NoC.hpp"
#include "Metrics/GlobalStats.hpp"
#include "DataStructs.hpp"
#include "Configuration/Graph/Graph.hpp"
#include "Configuration/Configuration.hpp"
#include "Metrics/ProgressBar.hpp"
#include <chrono>

const static std::string Version = "0.0.1.2";



int sc_main(int arg_num, char* arg_vet[]) {
	try {
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

		Network.reset.write(true);
		std::cout << "Reset for " << Config.ResetTime() << " cycles... ";
		sc_start(Config.ResetTime(), SC_NS);
		Network.reset.write(false);
		std::cout << " done!\n";

		std::cout << " Now running for " << Config.SimulationTime() << " cycles...\n";

		if (Config.ReportProgress()) std::cout << " Progress: ";
		auto start_time = std::chrono::high_resolution_clock::now();
		sc_start(Config.SimulationTime(), SC_NS);
		auto end_time = std::chrono::high_resolution_clock::now();
		if (Config.ReportProgress()) std::cout << '\n';

		std::cout 
			<< "Newxim simulation completed.\n"
			<< static_cast<std::int32_t>(Timer.SystemTime()) 
			<< " cycles executed in "
			<< std::chrono::duration<double>(end_time - start_time).count()
			<< "s\n\n";

		std::cout << stats;
		return 0;
	} catch (const std::exception& ex) {
		std::cout << "Error: " << ex.what() << '\n';
		return 1;
	}
}
