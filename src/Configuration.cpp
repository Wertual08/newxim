#include "Configuration.h"
#include <yaml-cpp/yaml.h>
#include <systemc.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include "Buffer.h"



namespace YAML
{
	template<>
	struct convert<Configuration::BufferPower>
	{
		static bool decode(const Node& node, Configuration::BufferPower& bufferPowerConfig)
		{
			for (YAML::const_iterator buffering_it = node.begin();
				buffering_it != node.end();
				++buffering_it)
			{
				std::vector<double> v = buffering_it->as<std::vector<double>>();
				//cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << " " << v[4] << " " << v[5] << endl;
				bufferPowerConfig.leakage[std::make_pair(v[0], v[1])] = v[2];
				bufferPowerConfig.push[std::make_pair(v[0], v[1])] = v[3];
				bufferPowerConfig.front[std::make_pair(v[0], v[1])] = v[4];
				bufferPowerConfig.pop[std::make_pair(v[0], v[1])] = v[5];
			}
			return true;
		}
	};

	template<>
	struct convert<std::map<double, std::pair<double, double>>>
	{
		static bool decode(const Node& node, std::map<double, std::pair<double, double>>& linkBitLinePowerConfig)
		{
			for (YAML::const_iterator link_bit_line_pc_it = node.begin();
				link_bit_line_pc_it != node.end();
				++link_bit_line_pc_it)
			{
				std::vector<double> v = link_bit_line_pc_it->as<std::vector<double> >();
				//cout << v[0] << " " << v[1] << " " << v[2] << endl;
				linkBitLinePowerConfig[v[0]] = std::make_pair(v[1], v[2]);
			}
			return true;
		}
	};


	template<>
	struct convert<Configuration::RouterPower>
	{
		static bool decode(const Node& node, Configuration::RouterPower& routerPowerConfig)
		{

			for (YAML::const_iterator crossbar_it = node["crossbar"].begin();
				crossbar_it != node["crossbar"].end();
				++crossbar_it)
			{
				std::vector<double> v = crossbar_it->as<std::vector<double> >();
				//cout << v[0] << " " << v[1] << " " << v[2] << " " << v[3] << endl;
				routerPowerConfig.crossbar_pm[std::make_pair(v[0], v[1])] = std::make_pair(v[2], v[3]);
			}

			for (YAML::const_iterator network_interface_it = node["network_interface"].begin();
				network_interface_it != node["network_interface"].end();
				++network_interface_it)
			{
				std::vector<double> v = network_interface_it->as<std::vector<double> >();
				//cout << v[0] << " " << v[1] << " " << v[2] << endl;
				routerPowerConfig.network_interface[v[0]] = std::make_pair(v[1], v[2]);
			}

			for (YAML::const_iterator routing_it = node["routing"].begin();
				routing_it != node["routing"].end();
				++routing_it)
			{
				routerPowerConfig.routing_algorithm_pm[routing_it->first.as<std::string>()] = routing_it->second.as<std::pair<double, double> >();
			}

			for (YAML::const_iterator selection_it = node["selection"].begin();
				selection_it != node["selection"].end();
				++selection_it)
			{
				routerPowerConfig.selection_strategy_pm[selection_it->first.as<std::string>()] = selection_it->second.as<std::pair<double, double> >();
			}

			return true;
		}
	};

	template<>
	struct convert<Configuration::Power> 
	{
		static bool decode(const Node& node, Configuration::Power& powerConfig)
		{
			powerConfig.bufferPowerConfig = node["Buffer"].as<Configuration::BufferPower>();
			powerConfig.linkBitLinePowerConfig = node["LinkBitLine"].as<std::map<double, std::pair<double, double>>>();
			powerConfig.routerPowerConfig = node["Router"].as<Configuration::RouterPower>();
			return true;
		}
	};
}
template <typename T>
T ReadParam(YAML::Node node, std::string param, T default_value)
{
	try 
	{
		return node[param].as<T>();
	}
	catch (std::exception& e) 
	{
		/*
		std::cerr << "WARNING: parameter " << param << " not present in YAML configuration file.\n";
		std::cerr << "Using command line value or default value " << default_value << '\n';
		 */
		return default_value;
	}
}
template <typename T>
T ReadParam(YAML::Node node, std::string param)
{
	try
	{
		return node[param].as<T>();
	}
	catch (std::exception& e) 
	{
		std::cerr << "ERROR: Cannot read param " << param << ". \n";
		exit(0);
	}
}


std::string Configuration::default_config_filename = "config.yaml";
std::string Configuration::default_power_config_filename = "power.yaml";

void Configuration::ShowHelp(const std::string& selfname)
{
	std::cout 
		<< "Usage: " << selfname << " [options]\n"
		<< "Where [options] is one or more of the following ones:\n"
		<< "\t-help\t\t\tShow this help and exit\n"
		<< "\t-config\t\t\tLoad the specified configuration file\n"
		<< "\t-power\t\t\tLoad the specified power configurations file\n"
		<< "\t-verbose N\t\tVerbosity level (1=low, 2=medium, 3=high)\n"
		<< "\t-trace FILENAME\t\tTrace signals to a VCD file named 'FILENAME.vcd'\n"
		<< "\t-buffer N\t\tSet the depth of router input buffers [flits]\n"
		<< "\t-vc N\t\t\tNumber of virtual channels\n"
		<< "\t-size Nmin Nmax\t\tSet the minimum and maximum packet size [flits]\n"
		<< "\t-flit N\t\t\tSet the flit size [bit]\n"
		<< "\t-topology TYPE\t\tSet the topology to one of the following:\n"
		<< "\t\tMESH\t\t2D Mesh\n"
		<< "\t\tBUTTERFLY\tDelta network Butterfly (radix 2)\n"
		<< "\t\tBASELINE\tDelta network Baseline\n"
		<< "\t\tOMEGA\t\tDelta network Omega\n"
		<< "\t-routing TYPE\t\tSet the routing algorithm to one of the following:\n"
		<< "\t\tXY\t\tXY routing algorithm\n"
		<< "\t\tWEST_FIRST\tWest-First routing algorithm\n"
		<< "\t\tNORTH_LAST\tNorth-Last routing algorithm\n"
		<< "\t\tNEGATIVE_FIRST\tNegative-First routing algorithm\n"
		<< "\t\tODD_EVEN\tOdd-Even routing algorithm\n"
		<< "\t\tDYAD T\t\tDyAD routing algorithm with threshold T\n"
		<< "\t\tTABLE_BASED FILENAME\tRouting Table Based routing algorithm with table in the specified file\n"
		<< "\t-sel TYPE\t\tSet the selection strategy to one of the following:\n"
		<< "\t\tRANDOM\t\tRandom selection strategy\n"
		<< "\t\tBUFFER_LEVEL\tBuffer-Level Based selection strategy\n"
		<< "\t\tNOP\t\tNeighbors-on-Path selection strategy\n"
		<< "\t-pir R TYPE\t\tSet the packet injection rate R [0..1] and the time distribution TYPE where TYPE is one of the following:\n"
		<< "\t\tpoisson\t\tMemory-less Poisson distribution\n"
		<< "\t\tburst R\t\tBurst distribution with given real burstness\n"
		<< "\t\tpareto on off r\tSelf-similar Pareto distribution with given real parameters (alfa-on alfa-off r)\n"
		<< "\t\tcustom R\tCustom distribution with given real probability of retransmission\n"
		<< "\t-traffic TYPE\t\tSet the spatial distribution of traffic to TYPE where TYPE is one of the following:\n"
		<< "\t\trandom\t\tRandom traffic distribution\n"
		<< "\t\tlocal L\t\tRandom traffic with a fraction L (0..1) of packets having a destination connected to the local hub, i.e. not using wireless\n"
		<< "\t\tulocal\t\tRandom traffic with locality smooth distribution\n"
		<< "\t\ttranspose1\tTranspose matrix 1 traffic distribution\n"
		<< "\t\ttranspose2\tTranspose matrix 2 traffic distribution\n"
		<< "\t\tbitreversal\tBit-reversal traffic distribution\n"
		<< "\t\tbutterfly\tButterfly traffic distribution\n"
		<< "\t\tshuffle\t\tShuffle traffic distribution\n"
		<< "\t\ttable FILENAME\tTraffic Table Based traffic distribution with table in the specified file\n"
		<< "\t-hs ID P\t\tAdd node ID to hotspot nodes, with percentage P (0..1) (Only for 'random' traffic)\n"
		<< "\t-warmup N\t\tStart to collect statistics after N cycles\n"
		<< "\t-seed N\t\t\tSet the seed of the random generator (default time())\n"
		<< "\t-detailed\t\tShow detailed statistics\n"
		<< "\t-show_buf_stats\t\tShow buffers statistics\n"
		<< "\t-volume N\t\tStop the simulation when either the maximum number of cycles has been reached or N flits have\n"
		<< "\t\t\t\tbeen delivered\n"
		<< "\t-sim N\t\t\tRun for the specified simulation time [cycles]\n"
		<< '\n'
		<< "If you find this program useful please don't forget to mention in your paper Maurizio Palesi <maurizio.palesi@unikore.it>\n"
		<< "If you find this program useless please feel free to complain with Davide Patti <davide.patti@dieei.unict.it>\n"
		<< "If you want to send money please feel free to PayPal to Fabrizio Fazzino <fabrizio@fazzino.it>\n"
		<< "and if need to solve any other problem of your life please contact Turi Monteleone <salvatore.monteleone@dieei.unict.it>\n";
}

Configuration::Configuration(int32_t arg_num, char* arg_vet[])
{
	bool config_found = false;
	bool power_config_found = false;

	for (int32_t i = 1; i < arg_num; i++)
	{
		if (!strcmp(arg_vet[i], "-help"))
		{
			ShowHelp(arg_vet[0]);
			exit(0);
		}
	}

	for (int32_t i = 1; i < arg_num; i++)
	{
		if (!strcmp(arg_vet[i], "-config"))
		{
			config_filename = arg_vet[++i];
			config_found = true;
			break;
		}
	}
	if (!config_found)
	{
		if (std::ifstream(default_config_filename).good()) config_filename = default_config_filename;
		else
		{
			std::cerr << "No YAML configuration file found!\n Use -config to load examples from config_examples folder\n";
			exit(0);
		}
	}

	for (int32_t i = 1; i < arg_num; i++)
	{
		if (!strcmp(arg_vet[i], "-power"))
		{
			power_config_filename = arg_vet[++i];
			power_config_found = true;
			break;
		}
	}
	if (!power_config_found)
	{
		if (std::ifstream(default_power_config_filename).good()) power_config_filename = default_power_config_filename;
		else
		{
			std::cerr << "No YAML power configurations file found!\n Use -power to load examples from config_examples folder\n";
			exit(0);
		}
	}

	YAML::Node config;
	YAML::Node power_config;
	std::cout << "Loading configuration from file \"" << config_filename << "\"...";
	try 
	{
		config = YAML::LoadFile(config_filename);
		std::cout << " Done\n";
	}
	catch (YAML::BadFile& e) {
		std::cout << " Failed\n";
		std::cerr << "The specified YAML configuration file was not found!\n";
		exit(0);
	}
	catch (YAML::ParserException& pe) {
		std::cout << " Failed\n";
		std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1 << ": " << pe.msg << ". Please check identation.\n";
		exit(0);
	}

	std::cout << "Loading power configurations from file \"" << power_config_filename << "\"...";
	try 
	{
		power_config = YAML::LoadFile(power_config_filename);
		std::cout << " Done\n";
	}
	catch (YAML::BadFile& e) 
	{
		std::cout << " Failed\n";
		std::cerr << "The specified YAML power configurations file was not found!\n";
		exit(0);
	}
	catch (YAML::ParserException& pe) 
	{
		std::cout << " Failed\n";
		std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1 << ": " << pe.msg << ". Please check identation.\n";
		exit(0);
	}

	// Initialize global configuration parameters (can be overridden with command-line arguments)
	verbose_mode = ReadParam<std::string>(config, "verbose_mode");
	trace_mode = ReadParam<bool>(config, "trace_mode");
	trace_filename = ReadParam<std::string>(config, "trace_filename");

	r2r_link_length = ReadParam<double>(config, "r2r_link_length");
	buffer_depth = ReadParam<int32_t>(config, "buffer_depth");
	flit_size = ReadParam<int32_t>(config, "flit_size");
	min_packet_size = ReadParam<int32_t>(config, "min_packet_size");
	max_packet_size = ReadParam<int32_t>(config, "max_packet_size");
	routing_algorithm = ReadParam<std::string>(config, "routing_algorithm");
	routing_table_filename = ReadParam<std::string>(config, "routing_table_filename");
	selection_strategy = ReadParam<std::string>(config, "selection_strategy");
	packet_injection_rate = ReadParam<double>(config, "packet_injection_rate");
	probability_of_retransmission = ReadParam<double>(config, "probability_of_retransmission");
	traffic_distribution = ReadParam<std::string>(config, "traffic_distribution");
	traffic_table_filename = ReadParam<std::string>(config, "traffic_table_filename");
	clock_period_ps = ReadParam<int32_t>(config, "clock_period_ps");
	simulation_time = ReadParam<int32_t>(config, "simulation_time");
	n_virtual_channels = ReadParam<int32_t>(config, "n_virtual_channels");
	reset_time = ReadParam<int32_t>(config, "reset_time");
	stats_warm_up_time = ReadParam<int32_t>(config, "stats_warm_up_time");
	rnd_generator_seed = 0/*time(0) // TODO: FIX THIS SHIT, zero seed is for testing only*/;
	detailed = ReadParam<bool>(config, "detailed");
	dyad_threshold = ReadParam<double>(config, "dyad_threshold");
	max_volume_to_be_drained = ReadParam<uint32_t>(config, "max_volume_to_be_drained");

	show_buffer_stats = ReadParam<bool>(config, "show_buffer_stats");
	use_powermanager = ReadParam<bool>(config, "use_wirxsleep");

	power_configuration = power_config["Energy"].as<Power>();

	ParseArgs(arg_num, arg_vet);

	Check();

	// Show configuration
	if (verbose_mode > VERBOSE_OFF) Show();
}
void Configuration::ParseArgs(int32_t arg_num, char* arg_vet[])
{
	if (arg_num == 1)
		std::cout << "Running with default parameters (use '-help' option to see how to override them)\n";
	else
	{
		for (int32_t i = 1; i < arg_num; i++)
		{
			if (!strcmp(arg_vet[i], "-verbose"))
				verbose_mode = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-trace"))
			{
				trace_mode = true;
				trace_filename = arg_vet[++i];
			}
			else if (!strcmp(arg_vet[i], "-buffer")) buffer_depth = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-vc")) n_virtual_channels = (atoi(arg_vet[++i]));
			else if (!strcmp(arg_vet[i], "-flit")) flit_size = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-wirxsleep")) use_powermanager = true;
			else if (!strcmp(arg_vet[i], "-size"))
			{
				min_packet_size = atoi(arg_vet[++i]);
				max_packet_size = atoi(arg_vet[++i]);
			}
			else if (!strcmp(arg_vet[i], "-routing"))
			{
				routing_algorithm = arg_vet[++i];
				//if (routing_algorithm == ROUTING_DYAD) dyad_threshold = atof(arg_vet[++i]);
				if (routing_algorithm == ROUTING_TABLE_BASED)
				{
					routing_table_filename = arg_vet[++i];
					packet_injection_rate = 0;
				}
			}
			else if (!strcmp(arg_vet[i], "-sel")) selection_strategy = arg_vet[++i];
			else if (!strcmp(arg_vet[i], "-pir"))
			{
				packet_injection_rate = atof(arg_vet[++i]);
				char* distribution = arg_vet[i + 1 < arg_num ? ++i : i];

				if (!strcmp(distribution, "poisson"))
					probability_of_retransmission = packet_injection_rate;
				else if (!strcmp(distribution, "burst"))
				{
					double burstness = atof(arg_vet[++i]);
					probability_of_retransmission = packet_injection_rate / (1 - burstness);
				}
				else if (!strcmp(distribution, "pareto")) 
				{
					double Aon = atof(arg_vet[++i]);
					double Aoff = atof(arg_vet[++i]);
					double r = atof(arg_vet[++i]);
					probability_of_retransmission = packet_injection_rate * pow((1 - r), (1 / Aoff - 1 / Aon));
				}
				else if (!strcmp(distribution, "custom")) probability_of_retransmission = atof(arg_vet[++i]);
				else assert("Invalid pir format" && false);
			}
			else if (!strcmp(arg_vet[i], "-traffic"))
			{
				char* traffic = arg_vet[++i];
				if (!strcmp(traffic, "random")) traffic_distribution = TRAFFIC_RANDOM;
				//else if (!strcmp(traffic, "transpose1")) traffic_distribution = TRAFFIC_TRANSPOSE1;
				//else if (!strcmp(traffic, "transpose2")) traffic_distribution = TRAFFIC_TRANSPOSE2;
				//else if (!strcmp(traffic, "bitreversal")) traffic_distribution = TRAFFIC_BIT_REVERSAL;
				//else if (!strcmp(traffic, "butterfly")) traffic_distribution = TRAFFIC_BUTTERFLY;
				//else if (!strcmp(traffic, "shuffle")) traffic_distribution = TRAFFIC_SHUFFLE;
				//else if (!strcmp(traffic, "ulocal")) traffic_distribution = TRAFFIC_ULOCAL;
				//else if (!strcmp(traffic, "table")) 
				//{
				//	traffic_distribution = TRAFFIC_TABLE_BASED;
				//	traffic_table_filename = arg_vet[++i];
				//}
				//else if (!strcmp(traffic, "local"))
				//{
				//	traffic_distribution = TRAFFIC_LOCAL;
				//	locality = atof(arg_vet[++i]);
				//}
				else assert(false);
			}
			else if (!strcmp(arg_vet[i], "-hs"))
			{
				int32_t node = atoi(arg_vet[++i]);
				double percentage = atof(arg_vet[++i]);
				std::pair<int32_t, double> t(node, percentage);
				hotspots.push_back(t);
			}
			else if (!strcmp(arg_vet[i], "-warmup")) stats_warm_up_time = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-seed")) rnd_generator_seed = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-detailed")) detailed = true;
			else if (!strcmp(arg_vet[i], "-show_buf_stats")) show_buffer_stats = true;
			else if (!strcmp(arg_vet[i], "-volume")) max_volume_to_be_drained = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-sim")) simulation_time = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-config") || !strcmp(arg_vet[i], "-power")) i++;
			else {
				std::cerr << "Error: Invalid option: " << arg_vet[i] << '\n';
				exit(1);
			}
		}
	}
}
void Configuration::Check()
{
	if (buffer_depth < 1) 
	{
		std::cerr << "Error: buffer must be >= 1\n";
		exit(1);
	}
	if (flit_size <= 0) 
	{
		std::cerr << "Error: flit_size must be > 0\n";
		exit(1);
	}

	if (min_packet_size < 2 || max_packet_size < 2) 
	{
		std::cerr << "Error: packet size must be >= 2\n";
		exit(1);
	}

	if (min_packet_size > max_packet_size) 
	{
		std::cerr << "Error: min packet size must be less than max packet size\n";
		exit(1);
	}

	if (selection_strategy.compare("INVALID_SELECTION") == 0) 
	{
		std::cerr << "Error: invalid selection policy\n";
		exit(1);
	}

	if (packet_injection_rate <= 0.0 || packet_injection_rate > 1.0) 
	{
		std::cerr << "Error: packet injection rate mmust be in the interval ]0,1]\n";
		exit(1);
	}

	for (unsigned int i = 0; i < hotspots.size(); i++) 
	{
		//if (topology == TOPOLOGY_MESH) {
		//	if (hotspots[i].first >=
		//		mesh_dim_x *
		//		mesh_dim_y) {
		//		std::cerr << "Error: hotspot node " << hotspots[i].first << " is invalid (out of range)\n";
		//		exit(1);
		//	}
		//}
		//else {
		//	if (hotspots[i].first >= n_delta_tiles) {
		//		std::cerr << "Error: hotspot node " << hotspots[i].first << " is invalid (out of range)\n";
		//		exit(1);
		//	}
		//}

		if (hotspots[i].second < 0.0 && hotspots[i].second > 1.0) 
		{
			std::cerr << "Error: hotspot percentage must be in the interval [0,1]\n";
			exit(1);
		}
	}

	if (stats_warm_up_time < 0) 
	{
		std::cerr << "Error: warm-up time must be positive\n";
		exit(1);
	}

	if (simulation_time < 0)
	{
		std::cerr << "Error: simulation time must be positive\n";
		exit(1);
	}
	if (n_virtual_channels > MAX_VIRTUAL_CHANNELS) 
	{
		std::cerr << "Error: number of virtual channels must be less than " << MAX_VIRTUAL_CHANNELS << endl;
		exit(1);
	}

	if (stats_warm_up_time > simulation_time) 
	{
		std::cerr << "Error: warmup time must be less than simulation time\n";
		exit(1);
	}

	if (locality < 0 || locality>1)
	{
		std::cerr << "Error: traffic locality must be in the range 0..1\n";
		exit(1);
	}


	if (n_virtual_channels > 1 && selection_strategy.compare("NOP") == 0)
	{
		std::cerr << "Error: NoP selection strategy can be used only with a single virtual channel\n";
		exit(1);
	}

	if (n_virtual_channels > 1 && selection_strategy.compare("BUFFER_LEVEL") == 0)
	{
		std::cerr << "Error: Buffer level selection strategy can be used only with a single virtual channel\n";
		exit(1);
	}
	if (n_virtual_channels > MAX_VIRTUAL_CHANNELS)
	{
		std::cerr << "Error: cannot use more than " << MAX_VIRTUAL_CHANNELS << " virtual channels.\n"
			<< "If you need more vc please modify the MAX_VIRTUAL_CHANNELS definition in \n"
			<< "GlobalParams.h and compile again \n";
		exit(1);
	}
	if (n_virtual_channels > 1 && use_powermanager)
	{
		std::cerr << "Error: Power manager (-wirxsleep) option only supports a single virtual channel\n";
		exit(1);
	}
}

void Configuration::Show() const
{
	std::cout 
		<< "Using the following configuration: \n"
		<< "- verbose_mode = " << verbose_mode << '\n'
		<< "- trace_mode = " << trace_mode << '\n'
		// << "- trace_filename = " << trace_filename << '\n'
		<< "- buffer_depth = " << buffer_depth << '\n'
		<< "- n_virtual_channels = " << n_virtual_channels << '\n'
		<< "- max_packet_size = " << max_packet_size << '\n'
		<< "- routing_algorithm = " << routing_algorithm << '\n'
		<< "- routing_table_filename = " << routing_table_filename << '\n'
		<< "- selection_strategy = " << selection_strategy << '\n'
		<< "- packet_injection_rate = " << packet_injection_rate << '\n'
		<< "- probability_of_retransmission = " << probability_of_retransmission << '\n'
		<< "- traffic_distribution = " << traffic_distribution << '\n'
		<< "- clock_period = " << clock_period_ps << "ps\n"
		<< "- simulation_time = " << simulation_time << '\n'
		<< "- warm_up_time = " << stats_warm_up_time << '\n'
		<< "- rnd_generator_seed = " << rnd_generator_seed << '\n';
}

const std::string& Configuration::VerboseMode() const
{
	return verbose_mode;
}
int32_t Configuration::TraceMode() const
{
	return trace_mode;
}
const std::string& Configuration::TraceFilename() const
{
	return trace_filename;
}
double Configuration::R2RLinkLength() const
{
	return r2r_link_length;
}
int32_t Configuration::BufferDepth() const
{
	return buffer_depth;
}
int32_t Configuration::FlitSize() const
{
	return flit_size;
}
int32_t Configuration::MinPacketSize() const
{
	return min_packet_size;
}
int32_t Configuration::MaxPacketSize() const
{
	return max_packet_size;
}
const std::string& Configuration::RoutingAlgorithm() const
{
	return routing_algorithm;
}
const std::string& Configuration::RoutingTableFilename() const
{
	return routing_table_filename;
}
const std::string& Configuration::SelectionStrategy() const
{
	return selection_strategy;
}
double Configuration::PacketInjectionRate() const
{
	return packet_injection_rate;
}
double Configuration::ProbabilityOfRetransmission() const
{
	return probability_of_retransmission;
}
double Configuration::Locality() const
{
	return locality;
}
const std::string& Configuration::TrafficDistribution() const
{
	return traffic_distribution;
}
const std::string& Configuration::TrafficTableFilename() const
{
	return traffic_table_filename;
}
const std::string& Configuration::ConfigFilename() const
{
	return config_filename;
}
const std::string& Configuration::PowerConfigFilename() const
{
	return power_config_filename;
}
int32_t Configuration::ClockPeriodPS() const
{
	return clock_period_ps;
}
int32_t Configuration::SimulationTime() const
{
	return simulation_time;
}
int32_t Configuration::NVirtualChannels() const
{
	return n_virtual_channels;
}
int32_t Configuration::ResetTime() const
{
	return reset_time;
}
int32_t Configuration::StatsWarmUpTime() const
{
	return stats_warm_up_time;
}
int32_t Configuration::RndGeneratorSeed() const
{
	return rnd_generator_seed;
}
bool Configuration::Detailed() const
{
	return detailed;
}
std::vector<std::pair<int32_t, double>> Configuration::Hotspots() const
{
	return hotspots;
}
double Configuration::DyadThreshold() const
{
	return dyad_threshold;
}
uint32_t Configuration::MaxVolumeToBeDrained() const
{
	return max_volume_to_be_drained;
}
bool Configuration::ShowBufferStats() const
{
	return show_buffer_stats;
}
bool Configuration::UsePowermanager() const
{
	return use_powermanager;
}
const Configuration::Power& Configuration::PowerConfiguration() const
{
	return power_configuration;
}

double Configuration::TimeStamp() const
{
	return sc_time_stamp().to_double() / clock_period_ps;
}
