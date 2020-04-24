/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the command line parser
 */

#include "ConfigurationManager.h"
#include <fstream>
#include <ctime>

YAML::Node config;
YAML::Node power_config;

template <typename T>
T readParam(YAML::Node node, std::string param, T default_value) {
	try
	{
		return node[param].as<T>();
	}
	catch (std::exception& e)
	{
		/*
		std::cerr << "WARNING: parameter " << param << " not present in YAML configuration file." << std::endl;
		std::cerr << "Using command line value or default value " << default_value << std::endl;
		 */
		return default_value;
	}
}

template <typename T>
T readParam(YAML::Node node, std::string param) {
	try {
		return node[param].as<T>();
	}
	catch (std::exception& e)
	{
		std::cerr << "ERROR: Cannot read param " << param << ". " << std::endl;
		exit(0);
	}
}

void loadConfiguration() 
{
	std::cout << "Loading configuration from file \"" << GlobalParams::config_filename << "\"...";
	try {
		config = YAML::LoadFile(GlobalParams::config_filename);
		std::cout << " Done" << std::endl;
	}
	catch (YAML::BadFile& e) {
		std::cout << " Failed" << std::endl;
		std::cerr << "The specified YAML configuration file was not found!" << std::endl;
		exit(0);
	}
	catch (YAML::ParserException& pe) {
		std::cout << " Failed" << std::endl;
		std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1 << ": " << pe.msg << ". Please check identation." << std::endl;
		exit(0);
	}

	std::cout << "Loading power configurations from file \"" << GlobalParams::power_config_filename << "\"...";
	try {
		power_config = YAML::LoadFile(GlobalParams::power_config_filename);
		std::cout << " Done" << std::endl;
	}
	catch (YAML::BadFile& e) {
		std::cout << " Failed" << std::endl;
		std::cerr << "The specified YAML power configurations file was not found!" << std::endl;
		exit(0);
	}
	catch (YAML::ParserException& pe) {
		std::cout << " Failed" << std::endl;
		std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1 << ": " << pe.msg << ". Please check identation." << std::endl;
		exit(0);
	}

	// Initialize global configuration parameters (can be overridden with command-line arguments)
	//GlobalParams::topology_filename = readParam<std::string>(config, "topology_filename");
	GlobalParams::verbose_mode = readParam<std::string>(config, "verbose_mode");
	GlobalParams::trace_mode = readParam<bool>(config, "trace_mode");
	GlobalParams::trace_filename = readParam<std::string>(config, "trace_filename");

	GlobalParams::r2r_link_length = readParam<double>(config, "r2r_link_length");
	GlobalParams::buffer_depth = readParam<int>(config, "buffer_depth");
	GlobalParams::flit_size = readParam<int>(config, "flit_size");
	GlobalParams::min_packet_size = readParam<int>(config, "min_packet_size");
	GlobalParams::max_packet_size = readParam<int>(config, "max_packet_size");
	GlobalParams::packet_injection_rate = readParam<double>(config, "packet_injection_rate");
	GlobalParams::probability_of_retransmission = readParam<double>(config, "probability_of_retransmission");
	GlobalParams::traffic_distribution = readParam<std::string>(config, "traffic_distribution");
	GlobalParams::traffic_table_filename = readParam<std::string>(config, "traffic_table_filename");
	GlobalParams::clock_period_ps = readParam<int>(config, "clock_period_ps");
	GlobalParams::simulation_time = readParam<int>(config, "simulation_time");
	GlobalParams::n_virtual_channels = readParam<int>(config, "n_virtual_channels");
	GlobalParams::reset_time = readParam<int>(config, "reset_time");
	GlobalParams::stats_warm_up_time = readParam<int>(config, "stats_warm_up_time");
	GlobalParams::rnd_generator_seed = time(0);
	GlobalParams::detailed = readParam<bool>(config, "detailed");
	GlobalParams::dyad_threshold = readParam<double>(config, "dyad_threshold");
	GlobalParams::max_volume_to_be_drained = readParam<unsigned int>(config, "max_volume_to_be_drained");
	//GlobalParams::hotspots;
	GlobalParams::show_buffer_stats = readParam<bool>(config, "show_buffer_stats");
	GlobalParams::use_powermanager = readParam<bool>(config, "use_wirxsleep");

	GlobalParams::power_configuration = power_config["Energy"].as<PowerConfig>();
}

void showHelp(char selfname[])
{
	std::cout << "Usage: " << selfname << " [options]" << std::endl
		<< "Where [options] is one or more of the following ones:" << std::endl
		<< "\t-help\t\t\tShow this help and exit" << std::endl
		<< "\t-config\t\t\tLoad the specified configuration file" << std::endl
		<< "\t-power\t\t\tLoad the specified power configurations file" << std::endl
		<< "\t-verbose N\t\tVerbosity level (1=low, 2=medium, 3=high)" << std::endl
		<< "\t-trace FILENAME\t\tTrace signals to a VCD file named 'FILENAME.vcd'" << std::endl
		<< "\t-buffer N\t\tSet the depth of router input buffers [flits]" << std::endl
		<< "\t-vc N\t\t\tNumber of virtual channels" << std::endl
		<< "\t-size Nmin Nmax\t\tSet the minimum and maximum packet size [flits]" << std::endl
		<< "\t-flit N\t\t\tSet the flit size [bit]" << std::endl
		<< "\t-topology TYPE\t\tSet the topology to one of the following:" << std::endl
		<< "\t\tMESH\t\t2D Mesh" << std::endl
		<< "\t\tBUTTERFLY\tDelta network Butterfly (radix 2)" << std::endl
		<< "\t\tBASELINE\tDelta network Baseline" << std::endl
		<< "\t\tOMEGA\t\tDelta network Omega" << std::endl
		<< "\t-routing TYPE\t\tSet the routing algorithm to one of the following:" << std::endl
		<< "\t\tXY\t\tXY routing algorithm" << std::endl
		<< "\t\tWEST_FIRST\tWest-First routing algorithm" << std::endl
		<< "\t\tNORTH_LAST\tNorth-Last routing algorithm" << std::endl
		<< "\t\tNEGATIVE_FIRST\tNegative-First routing algorithm" << std::endl
		<< "\t\tODD_EVEN\tOdd-Even routing algorithm" << std::endl
		<< "\t\tDYAD T\t\tDyAD routing algorithm with threshold T" << std::endl
		<< "\t\tTABLE_BASED FILENAME\tRouting Table Based routing algorithm with table in the specified file" << std::endl
		<< "\t-sel TYPE\t\tSet the selection strategy to one of the following:" << std::endl
		<< "\t\tRANDOM\t\tRandom selection strategy" << std::endl
		<< "\t\tBUFFER_LEVEL\tBuffer-Level Based selection strategy" << std::endl
		<< "\t\tNOP\t\tNeighbors-on-Path selection strategy" << std::endl
		<< "\t-pir R TYPE\t\tSet the packet injection rate R [0..1] and the time distribution TYPE where TYPE is one of the following:" << std::endl
		<< "\t\tpoisson\t\tMemory-less Poisson distribution" << std::endl
		<< "\t\tburst R\t\tBurst distribution with given real burstness" << std::endl
		<< "\t\tpareto on off r\tSelf-similar Pareto distribution with given real parameters (alfa-on alfa-off r)" << std::endl
		<< "\t\tcustom R\tCustom distribution with given real probability of retransmission" << std::endl
		<< "\t-traffic TYPE\t\tSet the spatial distribution of traffic to TYPE where TYPE is one of the following:" << std::endl
		<< "\t\trandom\t\tRandom traffic distribution" << std::endl
		<< "\t\tlocal L\t\tRandom traffic with a fraction L (0..1) of packets having a destination connected to the local hub, i.e. not using wireless" << std::endl
		<< "\t\tulocal\t\tRandom traffic with locality smooth distribution" << std::endl
		<< "\t\ttranspose1\tTranspose matrix 1 traffic distribution" << std::endl
		<< "\t\ttranspose2\tTranspose matrix 2 traffic distribution" << std::endl
		<< "\t\tbitreversal\tBit-reversal traffic distribution" << std::endl
		<< "\t\tbutterfly\tButterfly traffic distribution" << std::endl
		<< "\t\tshuffle\t\tShuffle traffic distribution" << std::endl
		<< "\t\ttable FILENAME\tTraffic Table Based traffic distribution with table in the specified file" << std::endl
		<< "\t-hs ID P\t\tAdd node ID to hotspot nodes, with percentage P (0..1) (Only for 'random' traffic)" << std::endl
		<< "\t-warmup N\t\tStart to collect statistics after N cycles" << std::endl
		<< "\t-seed N\t\t\tSet the seed of the random generator (default time())" << std::endl
		<< "\t-detailed\t\tShow detailed statistics" << std::endl
		<< "\t-show_buf_stats\t\tShow buffers statistics" << std::endl
		<< "\t-volume N\t\tStop the simulation when either the maximum number of cycles has been reached or N flits have" << std::endl
		<< "\t-topology FILENAME\t\tSpecify topology file" << std::endl
		<< "\t\t\t\tbeen delivered" << std::endl
		<< "\t-sim N\t\t\tRun for the specified simulation time [cycles]" << std::endl
		<< std::endl
		<< "If you find this program useful please don't forget to mention in your paper Maurizio Palesi <maurizio.palesi@unikore.it>" << std::endl
		<< "If you find this program useless please feel free to complain with Davide Patti <davide.patti@dieei.unict.it>" << std::endl
		<< "If you want to send money please feel free to PayPal to Fabrizio Fazzino <fabrizio@fazzino.it>" << std::endl
		<< "and if need to solve any other problem of your life please contact Turi Monteleone <salvatore.monteleone@dieei.unict.it>" << std::endl;
}

void showConfig()
{
	std::cout << "Using the following configuration: " << std::endl
		<< "- verbose_mode = " << GlobalParams::verbose_mode << std::endl
		<< "- trace_mode = " << GlobalParams::trace_mode << std::endl
		// << "- trace_filename = " << GlobalParams::trace_filename << std::endl
		<< "- buffer_depth = " << GlobalParams::buffer_depth << std::endl
		<< "- n_virtual_channels = " << GlobalParams::n_virtual_channels << std::endl
		<< "- max_packet_size = " << GlobalParams::max_packet_size << std::endl
		<< "- packet_injection_rate = " << GlobalParams::packet_injection_rate << std::endl
		<< "- probability_of_retransmission = " << GlobalParams::probability_of_retransmission << std::endl
		<< "- traffic_distribution = " << GlobalParams::traffic_distribution << std::endl
		<< "- clock_period = " << GlobalParams::clock_period_ps << "ps" << std::endl
		<< "- simulation_time = " << GlobalParams::simulation_time << std::endl
		<< "- warm_up_time = " << GlobalParams::stats_warm_up_time << std::endl
		<< "- rnd_generator_seed = " << GlobalParams::rnd_generator_seed << std::endl;
}

void checkConfiguration()
{
	if (GlobalParams::buffer_depth < 1) {
		std::cerr << "Error: buffer must be >= 1" << std::endl;
		exit(1);
	}
	if (GlobalParams::flit_size <= 0) {
		std::cerr << "Error: flit_size must be > 0" << std::endl;
		exit(1);
	}

	if (GlobalParams::min_packet_size < 2 ||
		GlobalParams::max_packet_size < 2) {
		std::cerr << "Error: packet size must be >= 2" << std::endl;
		exit(1);
	}

	if (GlobalParams::min_packet_size >
		GlobalParams::max_packet_size) {
		std::cerr << "Error: min packet size must be less than max packet size"
			<< std::endl;
		exit(1);
	}

	if (GlobalParams::packet_injection_rate <= 0.0 ||
		GlobalParams::packet_injection_rate > 1.0) {
		std::cerr <<
			"Error: packet injection rate mmust be in the interval ]0,1]"
			<< std::endl;
		exit(1);
	}

	for (unsigned int i = 0; i < GlobalParams::hotspots.size(); i++)
	{
		if (GlobalParams::hotspots[i].second < 0.0 && GlobalParams::hotspots[i].second > 1.0)
		{
			std::cerr <<
				"Error: hotspot percentage must be in the interval [0,1]"
				<< std::endl;
			exit(1);
		}
	}

	if (GlobalParams::stats_warm_up_time < 0) {
		std::cerr << "Error: warm-up time must be positive" << std::endl;
		exit(1);
	}

	if (GlobalParams::simulation_time < 0) {
		std::cerr << "Error: simulation time must be positive" << std::endl;
		exit(1);
	}
	if (GlobalParams::n_virtual_channels > MAX_VIRTUAL_CHANNELS) {
		std::cerr << "Error: number of virtual channels must be less than " << MAX_VIRTUAL_CHANNELS << std::endl;
		exit(1);
	}

	if (GlobalParams::stats_warm_up_time >
		GlobalParams::simulation_time) {
		std::cerr << "Error: warmup time must be less than simulation time" <<
			std::endl;
		exit(1);
	}

	if (GlobalParams::locality < 0 || GlobalParams::locality>1)
	{
		std::cerr << "Error: traffic locality must be in the range 0..1" << std::endl;
		exit(1);
	}

	if (GlobalParams::n_virtual_channels > MAX_VIRTUAL_CHANNELS)
	{
		std::cerr << "Error: cannot use more than " << MAX_VIRTUAL_CHANNELS << " virtual channels." << std::endl
			<< "If you need more vc please modify the MAX_VIRTUAL_CHANNELS definition in " << std::endl
			<< "GlobalParams.h and compile again " << std::endl;
		exit(1);
	}
	if (GlobalParams::n_virtual_channels > 1 && GlobalParams::use_powermanager)
	{
		std::cerr << "Error: Power manager (-wirxsleep) option only supports a single virtual channel" << std::endl;
		exit(1);
	}
}

void parseCmdLine(int arg_num, char* arg_vet[])
{
	if (arg_num == 1)
		std::cout <<
		"Running with default parameters (use '-help' option to see how to override them)"
		<< std::endl;
	else
	{
		for (int i = 1; i < arg_num; i++)
		{
			if (!strcmp(arg_vet[i], "-verbose"))
				GlobalParams::verbose_mode = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-trace"))
			{
				GlobalParams::trace_mode = true;
				GlobalParams::trace_filename = arg_vet[++i];
			}
			else if (!strcmp(arg_vet[i], "-buffer"))
				GlobalParams::buffer_depth = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-vc"))
				GlobalParams::n_virtual_channels = (atoi(arg_vet[++i]));
			else if (!strcmp(arg_vet[i], "-flit"))
				GlobalParams::flit_size = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-wirxsleep"))
			{
				GlobalParams::use_powermanager = true;
			}
			else if (!strcmp(arg_vet[i], "-size"))
			{
				GlobalParams::min_packet_size = atoi(arg_vet[++i]);
				GlobalParams::max_packet_size = atoi(arg_vet[++i]);
			}
			else if (!strcmp(arg_vet[i], "-pir"))
			{
				GlobalParams::packet_injection_rate = atof(arg_vet[++i]);
				char* distribution = arg_vet[i + 1 < arg_num ? ++i : i];

				if (!strcmp(distribution, "poisson"))
					GlobalParams::probability_of_retransmission = GlobalParams::packet_injection_rate;
				else if (!strcmp(distribution, "burst"))
				{
					double burstness = atof(arg_vet[++i]);
					GlobalParams::probability_of_retransmission = GlobalParams::packet_injection_rate / (1 - burstness);
				}
				else if (!strcmp(distribution, "pareto")) {
					double Aon = atof(arg_vet[++i]);
					double Aoff = atof(arg_vet[++i]);
					double r = atof(arg_vet[++i]);
					GlobalParams::probability_of_retransmission =
						GlobalParams::packet_injection_rate *
						pow((1 - r), (1 / Aoff - 1 / Aon));
				}
				else if (!strcmp(distribution, "custom"))
					GlobalParams::probability_of_retransmission = atof(arg_vet[++i]);
				else assert("Invalid pir format" && false);
			}
			else if (!strcmp(arg_vet[i], "-traffic"))
			{
				char* traffic = arg_vet[++i];
				if (!strcmp(traffic, "random")) GlobalParams::traffic_distribution = TRAFFIC_RANDOM;
				else if (!strcmp(traffic, "transpose1"))
					GlobalParams::traffic_distribution =
					TRAFFIC_TRANSPOSE1;
				else if (!strcmp(traffic, "transpose2"))
					GlobalParams::traffic_distribution =
					TRAFFIC_TRANSPOSE2;
				else if (!strcmp(traffic, "bitreversal"))
					GlobalParams::traffic_distribution =
					TRAFFIC_BIT_REVERSAL;
				else if (!strcmp(traffic, "butterfly"))
					GlobalParams::traffic_distribution =
					TRAFFIC_BUTTERFLY;
				else if (!strcmp(traffic, "shuffle"))
					GlobalParams::traffic_distribution =
					TRAFFIC_SHUFFLE;
				else if (!strcmp(traffic, "ulocal"))
					GlobalParams::traffic_distribution =
					TRAFFIC_ULOCAL;
				else if (!strcmp(traffic, "table")) {
					GlobalParams::traffic_distribution =
						TRAFFIC_TABLE_BASED;
					GlobalParams::traffic_table_filename = arg_vet[++i];
				}
				else if (!strcmp(traffic, "local")) {
					GlobalParams::traffic_distribution = TRAFFIC_LOCAL;
					GlobalParams::locality = atof(arg_vet[++i]);
				}
				else assert(false);
			}
			else if (!strcmp(arg_vet[i], "-hs"))
			{
				int node = atoi(arg_vet[++i]);
				double percentage = atof(arg_vet[++i]);
				std::pair<int, double> t(node, percentage);
				GlobalParams::hotspots.push_back(t);
			}
			else if (!strcmp(arg_vet[i], "-warmup"))
				GlobalParams::stats_warm_up_time = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-seed"))
				GlobalParams::rnd_generator_seed = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-detailed"))
				GlobalParams::detailed = true;
			else if (!strcmp(arg_vet[i], "-show_buf_stats"))
				GlobalParams::show_buffer_stats = true;
			else if (!strcmp(arg_vet[i], "-volume")) GlobalParams::max_volume_to_be_drained = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-sim")) GlobalParams::simulation_time = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-config") || !strcmp(arg_vet[i], "-power")) i++;
			else 
			{
				std::cerr << "Error: Invalid option: " << arg_vet[i] << std::endl;
				exit(1);
			}
		}
	}

}

void configure(int arg_num, char* arg_vet[]) {

	bool config_found = false;
	bool power_config_found = false;

	for (int i = 1; i < arg_num; i++)
	{
		if (!strcmp(arg_vet[i], "-help"))
		{
			showHelp(arg_vet[0]);
			exit(0);
		}
	}

	for (int i = 1; i < arg_num; i++)
	{
		if (!strcmp(arg_vet[i], "-config")) 
		{
			GlobalParams::config_filename = arg_vet[++i];
			config_found = true;
			break;
		}
	}
	if (!config_found)
	{
		if (std::ifstream(CONFIG_FILENAME).good()) GlobalParams::config_filename = CONFIG_FILENAME;
		else
		{
			std::cerr << "No YAML configuration file found!\n Use -config to load examples from config_examples folder" << std::endl;
			exit(0);
		}
	}

	for (int i = 1; i < arg_num; i++) 
	{
		if (!strcmp(arg_vet[i], "-power")) 
		{
			GlobalParams::power_config_filename = arg_vet[++i];
			power_config_found = true;
			break;
		}
	}
	if (!power_config_found)
	{
		if (std::ifstream(POWER_CONFIG_FILENAME).good()) GlobalParams::power_config_filename = POWER_CONFIG_FILENAME;
		else
		{
			std::cerr << "No YAML power configurations file found!\n Use -power to load examples from config_examples folder" << std::endl;
			exit(0);
		}
	}

	loadConfiguration();
	parseCmdLine(arg_num, arg_vet);

	checkConfiguration();

	// Show configuration
	if (GlobalParams::verbose_mode > VERBOSE_OFF) showConfig();
}
