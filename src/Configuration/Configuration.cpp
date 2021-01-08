#include "Configuration.hpp"
#include <yaml-cpp/yaml.h>
#include <systemc.h>
#include <iostream>
#include <fstream>
#include <cassert>
#include <ctime>
#include "Hardware/Buffer.hpp"
#include <filesystem>
#include <iomanip>



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

		std::string generator = ReadParam<std::string>(config, "subtopology", "NONE");
		if (generator != "NONE")
		{
			subgraph = graph.subgraph(generator);

			subtable = std::make_unique<RoutingTable>(subgraph);
			subtable->Promote(graph);

			virtual_subtable = std::make_unique<RoutingTable>(graph, subgraph);

			network_graph = graph + subgraph;
		}
		else network_graph = graph;
	}
	catch (...)
	{
		throw std::runtime_error("Configuration error: Failed to read topology.");
	}
}
void Configuration::ReadRouterParams(const YAML::Node& config)
{
	buffer_depth = ReadParam<std::int32_t>(config, "buffer_depth");
	flit_size = ReadParam<std::int32_t>(config, "flit_size");
	routing_algorithm = ReadParam<std::string>(config, "routing_algorithm");
	selection_strategy = ReadParam<std::string>(config, "selection_strategy");
}
void Configuration::ReadRoutingTableParams(const YAML::Node& config)
{
	try
	{
		auto node = config["routing_table"];
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
	report_routes_stats = ReadParam<bool>(config, "report_routes_stats", false);
	report_possible_routes = ReadParam<bool>(config, "report_possible_routes", false);
	report_topology_sub_graph = ReadParam<bool>(config, "report_topology_sub_graph", false);
	report_topology_sub_graph_adjacency_matrix = ReadParam<bool>(config, "report_topology_sub_graph_adjacency_matrix", false);
	report_sub_routing_table = ReadParam<bool>(config, "report_sub_routing_table", false);
	report_cycle_result = ReadParam<bool>(config, "report_cycle_result", false);

	flit_trace_start = -1;
	flit_trace_end = -1;
	auto node = config["report_flit_trace"];
	if (node.IsDefined())
	{
		if (node.IsSequence())
		{
			report_flit_trace = true;
			flit_trace_start = node[0].as<std::int32_t>();
			if (node.size() == 2) flit_trace_end = node[1].as<std::int32_t>();
		}
		else report_flit_trace = node.as<bool>();
	}
	else report_flit_trace = false;

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
	if (report_possible_routes)
	{
		std::cout << "Possible routes:\n";
		for (std::int32_t s = 0; s < graph.size(); s++)
		{
			for (std::int32_t d = 0; d < graph.size(); d++)
			{
				auto routes = table.GetPaths(graph, s, d);
				std::cout << std::setfill('0') << "[" << std::setw(2) << s << " -> " << std::setw(2) << d << "]:\n";
				for (std::int32_t i = 0; i < static_cast<std::int32_t>(routes.size()); i++)
				{
					const auto &route = routes[i];
					for (std::int32_t j = 0; j < static_cast<std::int32_t>(route.size()) - 1; j++)
					{
						std::cout << route[j] << " -> ";
					}
					if (route.size()) std::cout << route.back();
					std::cout << '\n';
				}
			}
		}
	}
	if (report_routes_stats)
	{
		std::size_t total_routes = 0;
		std::size_t total_distance = 0;
		std::size_t total_combinations = 0;
		std::size_t total_min_distance = 0;
		for (std::int32_t s = 0; s < graph.size(); s++)
		{
			for (std::int32_t d = 0; d < graph.size(); d++)
			{
				auto routes = table.GetPaths(graph, s, d);
				total_routes += routes.size();
				total_combinations++;
				std::size_t min_size = routes.front().size() - 1;
				for (const auto &route : routes)
				{
					total_distance += route.size() - 1;
					if (route.size() - 1 < min_size) min_size = route.size() - 1;
				}
				total_min_distance += min_size;
			}
		}
		std::cout << "Total routes:       " << total_routes << '\n';
		std::cout << "Total distance:     " << total_distance << '\n';
		std::cout << "Average distance:   " << static_cast<double>(total_distance) / static_cast<double>(total_routes) << '\n';
		std::cout << "Total combinations: " << total_combinations << '\n';
		std::cout << "Average min route:  " << static_cast<double>(total_min_distance) / static_cast<double>(total_combinations) << '\n';
	}
	if (report_topology_sub_graph) std::cout << "Topology subgraph: " << subgraph;
	if (report_topology_sub_graph_adjacency_matrix) std::cout << "Topology subgraph adjacency matrix:\n" << subgraph.adjacency_matrix();
	if (report_sub_routing_table && subtable) std::cout << "Subrouting table: " << *subtable << '\n';
}

void Configuration::ShowHelp(const std::string& selfname)
{
	std::cout 
		<< "Usage: " << selfname << " [options]\n"
		<< "-parameter_name parameter_value\n";
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


	ParseArgs(config, arg_num, arg_vet);


	ReadTopologyParams(config);
	ReadRouterParams(config);
	ReadRoutingTableParams(config);
	ReadSimulationParams(config);
	ReadTrafficDistributionParams(config);


	Check();

	ReportData();

	// Show configuration
	// Show();
}
void Configuration::ParseArgs(YAML::Node &node, std::int32_t arg_num, char* arg_vet[])
{
	if (arg_num == 1)
		std::cout << "Running with default parameters (use '-help' option to see how to override them)\n";
	else
	{
		for (std::int32_t i = 1; i < arg_num; i++)
		{
			if (arg_vet[i][0] == '-') {
				node[arg_vet[i] + 1] = arg_vet[++i];
			}
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
	return subgraph;
}
const Graph &Configuration::NetworkGraph() const
{
	return network_graph;
}
const RoutingTable& Configuration::GRTable() const
{
	return table;
}
const RoutingTable& Configuration::SubGRTable() const
{
	return *subtable;
}
const RoutingTable& Configuration::VirtualSubGRTable() const
{
	return *virtual_subtable;
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
bool Configuration::ReportFlitTrace() const
{
	return report_flit_trace;
}
double Configuration::FlitTraceStart() const
{
	return flit_trace_start;
}
double Configuration::FlitTraceEnd() const
{
	return flit_trace_end;
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
