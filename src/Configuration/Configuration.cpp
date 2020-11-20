#include "Configuration.hpp"
#include <yaml-cpp/yaml.h>
#include <systemc.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include "Hardware/Buffer.hpp"
#include <filesystem>



template <typename T>
T ReadParam(YAML::Node node, std::string param, T default_value)
{
	try
	{
		return node[param].as<T>();
	}
	catch (std::exception& e)
	{
		std::cerr << "WARNING: parameter " << param << " not present in YAML configuration file.\n";
		std::cerr << "Using command line value or default value " << default_value << '\n';
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
namespace YAML
{
	template<>
	struct convert<Configuration::Power::Buffer>
	{
		static bool decode(const Node& node, Configuration::Power::Buffer& bufferPowerConfig)
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
	struct convert<Configuration::Power::Router>
	{
		static bool decode(const Node& node, Configuration::Power::Router& routerPowerConfig)
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
			powerConfig.r2r_link_length = ReadParam<double>(node, "r2r_link_length");
			powerConfig.bufferPowerConfig = node["Energy"]["Buffer"].as<Configuration::Power::Buffer>();
			powerConfig.linkBitLinePowerConfig = node["Energy"]["LinkBitLine"].as<std::map<double, std::pair<double, double>>>();
			powerConfig.routerPowerConfig = node["Energy"]["Router"].as<Configuration::Power::Router>();
			return true;
		}
	};
}


std::string Configuration::default_config_filename = "config.yaml";
std::string Configuration::default_power_config_filename = "power.yaml";

void Configuration::ReadTopologyParams(const YAML::Node& config)
{
	std::string topology = ReadParam<std::string>(config, "topology");
	channels_count = ReadParam<std::int32_t>(config, "topology_channels", 1);
	virtual_channels_count = ReadParam<std::size_t>(config, "virtual_channels");
	try
	{
		const auto& args = config["topology_args"];
		if (topology == "CUSTOM")
		{
			for (std::int32_t i = 0; i < args.size(); i++)
			{
				const auto& branch = args[i];
				GraphNode gnode;
				for (std::int32_t j = 0; j < branch.size(); j++)
					gnode.push_back(branch[j].as<std::int32_t>());
				graph.push_back(std::move(gnode));
			}
		}
		else if (topology == "CIRCULANT")
		{
			graph.resize(args[0].as<std::int32_t>());
			for (std::int32_t i = 1; i < args.size(); i++)
			{
				std::int32_t l = args[i].as<std::int32_t>();
				for (std::int32_t j = 0; j < graph.size(); j++)
				{
					//if ((j + l) % graph.size() < j) continue;
					graph[j].push_back((j + l) % graph.size(), channels_count);
					graph[(j + l) % graph.size()].push_back(j, channels_count);
				}
			}
		}
		else if (topology == "MESH")
		{
			dim_x = args[0].as<std::int32_t>();
			dim_y = args[1].as<std::int32_t>();

			graph.resize(dim_x * dim_y);
			for (std::int32_t x = 0; x < dim_x; x++)
			{
				for (std::int32_t y = 0; y < dim_y; y++)
				{
					if (y + 1 < dim_y) graph[y * dim_x + x].push_back((y + 1) * dim_x + x, channels_count);
					if (x - 1 >= 0) graph[y * dim_x + x].push_back(y * dim_x + x - 1, channels_count);
					if (y - 1 >= 0) graph[y * dim_x + x].push_back((y - 1) * dim_x + x, channels_count);
					if (x + 1 < dim_x) graph[y * dim_x + x].push_back(y * dim_x + x + 1, channels_count);
				}
			}
		}
		else if (topology == "TORUS")
		{
			dim_x = args[0].as<std::int32_t>();
			dim_y = args[1].as<std::int32_t>();

			graph.resize(dim_x * dim_y);
			for (std::int32_t x = 0; x < dim_x; x++)
			{
				for (std::int32_t y = 0; y < dim_y; y++)
				{
					auto& node = graph[y * dim_x + x];
					if (y + 1 < dim_y) node.push_back((y + 1) * dim_x + x, channels_count);
					else node.push_back(x, channels_count);
					if (x - 1 >= 0) node.push_back(y * dim_x + x - 1, channels_count);
					else node.push_back(y * dim_x + dim_x - 1, channels_count);
					if (y - 1 >= 0) node.push_back((y - 1) * dim_x + x, channels_count);
					else node.push_back((dim_y - 1) * dim_x + x, channels_count);
					if (x + 1 < dim_x) node.push_back(y * dim_x + x + 1, channels_count);
					else node.push_back(y * dim_x, channels_count);
				}
			}
		}
		else if (topology == "TREE")
		{
			std::int32_t nodes_count = args[0].as<std::int32_t>();
			std::int32_t subnodes_count = args[1].as<std::int32_t>();
			graph.resize(nodes_count);
			for (std::int32_t i = 0; i < graph.size(); i++)
			{
				if (i * subnodes_count + 1 >= graph.size()) break;
				for (std::int32_t j = 1; j <= subnodes_count; j++)
				{
					if (i * subnodes_count + j >= graph.size()) break;
					graph[i].push_back(i * subnodes_count + j, channels_count);
					graph[i * subnodes_count + j].push_back(i, channels_count);
				}
			}
		}
		else if (topology == "TMESH")
		{
			dim_x = args[0].as<std::int32_t>();
			dim_y = args[1].as<std::int32_t>();

			graph.resize(dim_x * dim_y);
			for (std::int32_t x = 0; x < dim_x; x++)
			{
				for (std::int32_t y = 0; y < dim_y; y++)
				{
					if (y + 1 < dim_y) graph[y * dim_x + x].push_back((y + 1) * dim_x + x, channels_count);
					if (x - 1 >= 0) graph[y * dim_x + x].push_back(y * dim_x + x - 1, channels_count);
					if (y - 1 >= 0) graph[y * dim_x + x].push_back((y - 1) * dim_x + x, channels_count);
					if (x + 1 < dim_x) graph[y * dim_x + x].push_back(y * dim_x + x + 1, channels_count);

					if (x + 1 < dim_x && y + 1 < dim_y) graph[y * dim_x + x].push_back((y + 1) * dim_x + (x + 1), channels_count);
					if (x - 1 >= 0 && y + 1 < dim_y) graph[y * dim_x + x].push_back((y + 1) * dim_x + (x - 1), channels_count);
					if (x - 1 >= 0 && y - 1 >= 0) graph[y * dim_x + x].push_back((y - 1) * dim_x + (x - 1), channels_count);
					if (x + 1 < dim_x && y - 1 >= 0) graph[y * dim_x + x].push_back((y - 1) * dim_x + (x + 1), channels_count);
				}
			}
		}
		else
		{
			std::cerr << "ERROR: Unsupported topology.\n";
			exit(0);
		}
	}
	catch (...)
	{
		throw std::runtime_error("Configuration error: Failed to read topology.");
	}
}
void Configuration::ReadRouterParams(const YAML::Node& config)
{
	router_type = ReadParam<std::string>(config, "router_type");
	buffer_depth = ReadParam<std::int32_t>(config, "buffer_depth");
	flit_size = ReadParam<std::int32_t>(config, "flit_size");
	routing_algorithm = ReadParam<std::string>(config, "routing_algorithm");
	selection_strategy = ReadParam<std::string>(config, "selection_strategy");

	std::string generator = ReadParam<std::string>(config, "sub_tree_generator", "NONE");
	if (generator != "NONE")
	{
		sub_graph = std::make_unique<Graph>(graph.subtree(generator));
		sub_table = std::make_unique<RoutingTable>(*sub_graph);
		virtual_sub_table = std::make_unique<RoutingTable>(graph, *sub_graph);
	}
}
void Configuration::ReadRoutingTableParams(const YAML::Node& config)
{
	try
	{
		const auto& node = config["routing_table"];
		if (node.IsDefined())
		{
			if (node.IsSequence())
			{
				bool id_based = false;
				if (config["routing_table_id_based"].IsDefined())
					id_based = config["routing_table_id_based"].as<bool>();
				if (id_based)
				{
					for (std::int32_t i = 0; i < node.size(); i++)
					{
						const auto& branch = node[i];
						RoutingTable::Node rnode;
						for (std::int32_t j = 0; j < branch.size(); j++)
						{
							if (j == i) rnode.push_back(std::vector<std::int32_t>(1, graph[i].size()));
							else if (branch[j].IsSequence())
							{
								rnode.push_back(std::vector<std::int32_t>());
								for (std::int32_t k = 0; k < branch[j].size(); k++)
								{
									auto links = graph[i].links_to(branch[j][k].as<std::int32_t>());
									for (std::int32_t l : links) rnode[j].push_back(l);
								}
							}
							else rnode.push_back(graph[i].links_to(branch[j].as<std::int32_t>()));
						}
						table.push_back(std::move(rnode));
					}
				}
				else
				{
					for (std::int32_t i = 0; i < node.size(); i++)
					{
						const auto& branch = node[i];
						RoutingTable::Node rnode;
						for (std::int32_t j = 0; j < branch.size(); j++)
						{
							if (branch[j].IsSequence())
							{
								rnode.push_back(std::vector<std::int32_t>());
								for (std::int32_t k = 0; k < branch[j].size(); k++)
									rnode[j].push_back(branch[j][k].as<std::int32_t>());
							}
							else rnode.push_back(std::vector<std::int32_t>(1, branch[j].as<std::int32_t>()));
						}
						table.push_back(std::move(rnode));
					}
				}
			}
			else
			{
				std::string type = node.as<std::string>();
				if (!table.Load(graph, type))
					{
						std::cerr << "ERROR: Failed to generate routing table. \n";
						exit(0);
					}
			}
		}
		else table.Load(graph);
	}
	catch (...)
	{
		throw std::runtime_error("Configuration error: Failed to read routing table.");
	}
}
void Configuration::ReadSimulationParams(const YAML::Node& config)
{
	rnd_generator_seed = ReadParam<std::int32_t>(config, "rnd_generator_seed", time(0));
	report_progress = ReadParam<bool>(config, "report_progress", false);
	report_buffers = ReadParam<bool>(config, "report_buffers", false);
	report_topology_graph = ReadParam<bool>(config, "report_topology_graph", false);
	report_topology_graph_adjacency_matrix = ReadParam<bool>(config, "report_topology_graph_adjacency_matrix", false);
	report_routing_table = ReadParam<bool>(config, "report_routing_table", false);
	report_topology_sub_graph = ReadParam<bool>(config, "report_topology_sub_graph", false);
	report_topology_sub_graph_adjacency_matrix = ReadParam<bool>(config, "report_topology_sub_graph_adjacency_matrix", false);
	report_sub_routing_table = ReadParam<bool>(config, "report_sub_routing_table", false);
	report_cycle_result = ReadParam<bool>(config, "report_cycle_result", false);

	clock_period_ps = ReadParam<std::int32_t>(config, "clock_period_ps");
	reset_time = ReadParam<std::int32_t>(config, "reset_time");
	simulation_time = ReadParam<std::int32_t>(config, "simulation_time");
	stats_warm_up_time = ReadParam<std::int32_t>(config, "stats_warm_up_time");

	min_packet_size = ReadParam<std::int32_t>(config, "min_packet_size");
	max_packet_size = ReadParam<std::int32_t>(config, "max_packet_size");
	flit_injection_rate = ReadParam<bool>(config, "flit_injection_rate", false);
	scale_with_nodes = ReadParam<bool>(config, "scale_with_nodes", false);
	packet_injection_rate = ReadParam<double>(config, "packet_injection_rate");
	probability_of_retransmission = ReadParam<double>(config, "probability_of_retransmission");
}
void Configuration::ReadTrafficDistributionParams(const YAML::Node& config)
{

	traffic_distribution = ReadParam<std::string>(config, "traffic_distribution");
	if (traffic_distribution == "TRAFFIC_TABLE_BASED")
		traffic_table_filename = ReadParam<std::string>(config, "traffic_table_filename");
	else if (traffic_distribution == "TRAFFIC_HOTSPOT")
	{
		const auto& traffic_hotspots = config["traffic_hotspots"];
		for (std::int32_t i = 0; i < traffic_hotspots.size(); i++)
		{
			const auto& hotspot = traffic_hotspots[i];
			hotspots.push_back(std::make_pair(hotspot[0].as<std::int32_t>(),
				std::make_pair(hotspot[1].as<std::int32_t>(), hotspot[2].as<std::int32_t>())));
		}
	}
}

void Configuration::ReportData()
{
	if (report_topology_graph) std::cout << "Topology graph: " << graph;
	if (report_topology_graph_adjacency_matrix) std::cout << "Topology graph adjacency matrix:\n" << graph.adjacency_matrix();
	if (report_routing_table) std::cout << "Routing table: " << table << '\n'; 
	if (report_topology_sub_graph && sub_graph) std::cout << "Topology sub graph: " << *sub_graph;
	if (report_topology_sub_graph_adjacency_matrix && sub_graph) std::cout << "Topology sub graph adjacency matrix:\n" << sub_graph->adjacency_matrix();
	if (report_sub_routing_table && sub_table) std::cout << "Sub routing table: " << *sub_table << '\n';
}

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
		<< "\t-deFlitType::Tailed\t\tShow deFlitType::Tailed statistics\n"
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

Configuration::Configuration(std::int32_t arg_num, char* arg_vet[])
{
	// TODO: Maybe I should make it beautiful... But later

	for (std::int32_t i = 1; i < arg_num; i++)
	{
		if (!strcmp(arg_vet[i], "-help"))
		{
			ShowHelp(arg_vet[0]);
			exit(0);
		}
	}

	YAML::Node config;
	std::string config_filename = default_config_filename;
	for (std::int32_t i = 1; i < arg_num; i++)
	{
		if (!strcmp(arg_vet[i], "-config"))
		{
			config_filename = arg_vet[++i];
			break;
		}
	}
	std::cout << "Loading configuration from file \"" << config_filename << "\"...";
	try
	{
		config = YAML::LoadFile(config_filename);
		std::cout << " Done\n";
	}
	catch (YAML::BadFile& e) {
		std::cout << " Failed\n";
		std::cerr << "The specified YAML configuration file was not found!\nUse -config to load examples from config_examples folder.\n";
		exit(0);
	}
	catch (YAML::ParserException& pe) {
		std::cout << " Failed\n";
		std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1 << ": " << pe.msg << ". Please check identation.\n";
		exit(0);
	}

	ReadTopologyParams(config);
	ReadRouterParams(config);
	ReadRoutingTableParams(config);
	ReadSimulationParams(config);
	ReadTrafficDistributionParams(config);


	YAML::Node power_config;
	std::string power_config_filename = default_power_config_filename;
	for (std::int32_t i = 1; i < arg_num; i++)
	{
		if (!strcmp(arg_vet[i], "-power"))
		{
			power_config_filename = arg_vet[++i];
			break;
		}
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
		std::cerr << "The specified YAML power configurations file was not found!\nUse -power to load examples from config_examples folder.\n";
		exit(0);
	}
	catch (YAML::ParserException& pe)
	{
		std::cout << " Failed\n";
		std::cerr << "ERROR at line " << pe.mark.line + 1 << " column " << pe.mark.column + 1 << ": " << pe.msg << ". Please check identation.\n";
		exit(0);
	}
	power_configuration = power_config.as<Power>();

	ParseArgs(arg_num, arg_vet); 

	Check();

	ReportData();

	// Show configuration
	// Show();
}
void Configuration::ParseArgs(std::int32_t arg_num, char* arg_vet[])
{
	if (arg_num == 1)
		std::cout << "Running with default parameters (use '-help' option to see how to override them)\n";
	else
	{
		for (std::int32_t i = 1; i < arg_num; i++)
		{
			if (!strcmp(arg_vet[i], "-buffer")) buffer_depth = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-flit")) flit_size = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-size"))
			{
				min_packet_size = atoi(arg_vet[++i]);
				max_packet_size = atoi(arg_vet[++i]);
			}
			else if (!strcmp(arg_vet[i], "-routing"))
			{
				routing_algorithm = arg_vet[++i];
				if (routing_algorithm == "TABLE_BASED")
				{
					//routing_table_filename = arg_vet[++i];
					//packet_injection_rate = 0;
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
				if (!strcmp(traffic, "random")) traffic_distribution = "TRAFFIC_RANDOM";
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
			else if (!strcmp(arg_vet[i], "-warmup")) stats_warm_up_time = atoi(arg_vet[++i]);
			else if (!strcmp(arg_vet[i], "-seed")) rnd_generator_seed = atoi(arg_vet[++i]);
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

	if (min_packet_size < 1 || max_packet_size < 1) 
	{
		std::cerr << "Error: packet size must be >= 1\n";
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

	if (packet_injection_rate < 0.0 || packet_injection_rate > 
		(flit_injection_rate ? (min_packet_size + max_packet_size) / 2.0 : 1.0) * 
		(scale_with_nodes ? graph.size() : 1.0)) 
	{
		std::cerr << "Error: packet injection rate mmust be in the interval [0," << 
			(flit_injection_rate ? (min_packet_size + max_packet_size) / 2.0 : 1.0) *
			(scale_with_nodes ? graph.size() : 1.0) << "]\n";
		exit(1);
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
}

void Configuration::Show() const
{
	std::cout 
		<< "Using the following configuration: \n"
		<< "- buffer_depth = " << buffer_depth << '\n'
		<< "- max_packet_size = " << max_packet_size << '\n'
		<< "- routing_algorithm = " << routing_algorithm << '\n'
		<< "- selection_strategy = " << selection_strategy << '\n'
		<< "- packet_injection_rate = " << packet_injection_rate << '\n'
		<< "- probability_of_retransmission = " << probability_of_retransmission << '\n'
		<< "- traffic_distribution = " << traffic_distribution << '\n'
		<< "- clock_period = " << clock_period_ps << "ps\n"
		<< "- simulation_time = " << simulation_time << '\n'
		<< "- warm_up_time = " << stats_warm_up_time << '\n'
		<< "- rnd_generator_seed = " << rnd_generator_seed << '\n';
}

const Graph& Configuration::TopologyGraph() const
{
	return graph;
}
const Graph& Configuration::TopologySubGraph() const
{
	return *sub_graph;
}
const RoutingTable& Configuration::GRTable() const
{
	return table;
}
const RoutingTable& Configuration::SubGRTable() const
{
	return *sub_table;
}
const RoutingTable& Configuration::VirtualSubGRTable() const
{
	return *virtual_sub_table;
}
bool Configuration::Subnetwork() const
{
	return sub_graph && sub_table;
}
const std::vector<std::pair<std::int32_t, std::pair<std::int32_t, std::int32_t>>>& Configuration::Hotspots() const
{
	return hotspots;
}

std::int32_t Configuration::BufferDepth() const
{
	return buffer_depth;
}
std::int32_t Configuration::FlitSize() const
{
	return flit_size;
}
std::int32_t Configuration::MinPacketSize() const
{
	return min_packet_size;
}
std::int32_t Configuration::MaxPacketSize() const
{
	return max_packet_size;
}
const std::string& Configuration::RouterType() const
{
	return router_type;
}
const std::string& Configuration::RoutingAlgorithm() const
{
	return routing_algorithm;
}
const std::string& Configuration::SelectionStrategy() const
{
	return selection_strategy;
}
double Configuration::PacketInjectionRate() const
{
	if (!flit_injection_rate) return packet_injection_rate / (scale_with_nodes ? graph.size() : 1.0);
	else return packet_injection_rate * 2.0 / (min_packet_size + max_packet_size) / (scale_with_nodes ? graph.size() : 1.0);
}
double Configuration::ProbabilityOfRetransmission() const
{
	if (!flit_injection_rate) return probability_of_retransmission / (scale_with_nodes ? graph.size() : 1.0);
	else return probability_of_retransmission * 2.0 / (min_packet_size + max_packet_size) / (scale_with_nodes ? graph.size() : 1.0);
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
std::int32_t Configuration::ClockPeriodPS() const
{
	return clock_period_ps;
}
std::int32_t Configuration::SimulationTime() const
{
	return simulation_time;
}
std::int32_t Configuration::ResetTime() const
{
	return reset_time;
}
std::int32_t Configuration::StatsWarmUpTime() const
{
	return stats_warm_up_time;
}
std::int32_t Configuration::RndGeneratorSeed() const
{
	return rnd_generator_seed;
}
const Configuration::Power& Configuration::PowerConfiguration() const
{
	return power_configuration;
}
bool Configuration::ReportProgress() const
{
	return report_progress;
}
bool Configuration::ReportBuffers() const
{
	return report_buffers;
}
bool Configuration::ReportCycleResult() const
{
	return report_cycle_result;
}

std::int32_t Configuration::DimX() const
{
	return dim_x;
}
std::int32_t Configuration::DimY() const
{
	return dim_y;
}
std::int32_t Configuration::ChannelsCount() const
{
	return channels_count;
}
std::size_t Configuration::VirtualChannels() const
{
	return virtual_channels_count;
}

double Configuration::TimeStamp() const
{
	return sc_time_stamp().to_double() / clock_period_ps - reset_time;
}
