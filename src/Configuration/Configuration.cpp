#include "Configuration.hpp"
#include <yaml-cpp/yaml.h>
#include <systemc.h>
#include <iostream>
#include <fstream>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "Hardware/Buffer.hpp"
#include "Graph/CirculantGraph.hpp"
#include "Graph/MeshGraph.hpp"
#include "Graph/TorusGraph.hpp"
#include "Graph/TreeGraph.hpp"


template <typename T>
T ReadParam(const YAML::Node& node, const std::string& param) {
	try {
		return node[param].as<T>();
	} catch (const std::exception& e) {
		throw std::runtime_error((std::stringstream()
			<< "Unable to parse parameter ["
			<< param
			<< "]."
		).str());
	}
}


std::string Configuration::default_config_filename = "config.yml";


void Configuration::ReadTopologyParams(const YAML::Node& config) {
	std::string topology = ReadParam<std::string>(config, "topology");
	
	channels_count = ReadParam<std::int32_t>(config, "topology_channels");
	if (channels_count < 1) {
		throw std::runtime_error("topology_channels can not be less than 1.");
	}

	virtual_channels_count = ReadParam<std::size_t>(config, "virtual_channels");
	if (virtual_channels_count < 1) {
		throw std::runtime_error("virtual_channels can not be less than 1.");
	}

	const auto& args = config["topology_args"];
	if (topology == "CUSTOM") {
		for (std::int32_t i = 0; i < args.size(); i++) {
			const auto& branch = args[i];
			GraphNode gnode;
			for (std::int32_t j = 0; j < branch.size(); j++) {
				gnode.push_back(branch[j].as<std::int32_t>());
			}
			graph.push_back(std::move(gnode));
		}
	} else if (topology == "CIRCULANT") {
		std::vector<std::int32_t> generators(args.size() - 1);
		for (std::int32_t i = 1; i < args.size(); i++) {
			generators[i - 1] = args[i].as<std::int32_t>();
		}
		graph = CirculantGraph(
			args[0].as<std::int32_t>(), 
			generators, 
			channels_count
		);
	} else if (topology == "MESH") {
		graph = MeshGraph(
			dim_x = args[0].as<std::int32_t>(),
			dim_y = args[1].as<std::int32_t>(),
			channels_count
		);
	} else if (topology == "TORUS") {
		graph = TorusGraph(
			dim_x = args[0].as<std::int32_t>(),
			dim_y = args[1].as<std::int32_t>(),
			channels_count
		);
	} else if (topology == "TREE") {
		graph = TreeGraph(
			args[0].as<std::int32_t>(),
			args[1].as<std::int32_t>(),
			channels_count
		);
	} else {
		throw std::runtime_error((std::stringstream()
			<< "Unsupported topology ["
			<< topology
			<< "]."
		).str());
	}

	std::string generator = ReadParam<std::string>(config, "subtopology");
	std::string subnetwork = ReadParam<std::string>(config, "subnetwork");
	if (generator != "NONE") {
		subgraph = graph.subgraph(generator);
		subtable.Init(subgraph);
		subtable.LoadDijkstra(subgraph);

		if (subnetwork == "NONE") {
			network_graph = graph;
			subtable.Adjust(subgraph, graph);
		} else if (subnetwork == "VIRTUAL") {
			network_graph = graph;
			subtable.Adjust(subgraph, graph);
		} else if (subnetwork == "PHYSICAL") {
			network_graph = graph + subgraph;
			subtable.Promote(graph);
		} else {
			throw std::runtime_error((std::stringstream()
				<< "Unsupported subnetwork type ["
				<< subnetwork
				<< "]."
			).str());
		}
	} else {
		network_graph = graph;
	}
}
void Configuration::ReadRouterParams(const YAML::Node& config) {
	auto& node = config["update_sequence"];
	if (node.IsSequence()) {
		for (auto& sub_node : node) {
			update_sequence.push_back(sub_node.as<std::int32_t>());
		}
	}

	buffer_depth = ReadParam<std::int32_t>(config, "buffer_depth");
	if (buffer_depth < 1) {
		throw std::runtime_error("buffer_depth can not be less than 1.");
	}
	routing_algorithm = ReadParam<std::string>(config, "routing_algorithm");
	selection_strategy = ReadParam<std::string>(config, "selection_strategy");
}
void Configuration::ReadRoutingTableParams(const YAML::Node& config) {
	try {
		auto node = config["routing_table"];
		if (node.IsSequence()) {
			bool id_based = false;
			if (config["routing_table_id_based"].IsDefined()) {
				id_based = config["routing_table_id_based"].as<bool>();
			}
			if (id_based) {
				for (std::int32_t i = 0; i < node.size(); i++) {
					const auto& branch = node[i];
					RoutingTable::Node rnode;
					for (std::int32_t j = 0; j < branch.size(); j++) {
						if (j == i) {
							rnode.push_back(std::vector<std::int32_t>(1, graph[i].size()));
						} else if (branch[j].IsSequence()) {
							rnode.push_back(std::vector<std::int32_t>());
							for (std::int32_t k = 0; k < branch[j].size(); k++) {
								auto links = graph[i].links_to(branch[j][k].as<std::int32_t>());
								for (std::int32_t l : links) {
									rnode[j].push_back(l);
								}
							}
						} else {
							rnode.push_back(graph[i].links_to(branch[j].as<std::int32_t>()));
						}
					}
					table.push_back(std::move(rnode));
				}
			} else {
				for (std::int32_t i = 0; i < node.size(); i++) {
					const auto& branch = node[i];
					RoutingTable::Node rnode;
					for (std::int32_t j = 0; j < branch.size(); j++) {
						if (branch[j].IsSequence()) {
							rnode.push_back(std::vector<std::int32_t>());
							for (std::int32_t k = 0; k < branch[j].size(); k++) {
								rnode[j].push_back(branch[j][k].as<std::int32_t>());
							}
						} else {
							rnode.push_back(std::vector<std::int32_t>(1, branch[j].as<std::int32_t>()));
						}
					}
					table.push_back(std::move(rnode));
				}
			}
		} else {
			std::string type = ReadParam<std::string>(config, "routing_table");
			table.Init(graph);

			if (type == "DIJKSTRA") {
				table.LoadDijkstra(graph);
			} else if (type == "UP_DOWN") {
				table.LoadUpDown(graph);
			} else if (type == "MESH_XY") {
				table.LoadMeshXY(graph);
			} else if (type == "CIRCULANT_PAIR_EXCHANGE") {
				table.LoadCirculantPairExchange(graph);
			} else if (type == "CIRCULANT_MULTIPLICATIVE") {
				table.LoadCirculantMultiplicative(graph);
			} else if (type == "CIRCULANT_CLOCKWISE") {
				table.LoadCirculantClockwise(graph);
			} else if (type == "CIRCULANT_ADAPTIVE") {
				table.LoadCirculantAdaptive(graph);
			} else if (type == "GREEDY_PROMOTION") {
				table.LoadGreedyPromotion(graph);
			} else {
				throw std::runtime_error((std::stringstream()
					<< "Unsupported routing_table ["
					<< type
					<< "]."
				).str());
			}
		}
	} catch (...) {
		throw std::runtime_error("Failed to read routing table.");
	}
}
void Configuration::ReadSimulationParams(const YAML::Node& config) {
	rnd_generator_seed = ReadParam<std::int32_t>(config, "rnd_generator_seed");
	report_progress = ReadParam<bool>(config, "report_progress");
	json_result = ReadParam<bool>(config, "json_result");
	report_buffers = ReadParam<bool>(config, "report_buffers");
	report_topology_graph = ReadParam<bool>(config, "report_topology_graph");
	report_topology_graph_adjacency_matrix = ReadParam<bool>(config, "report_topology_graph_adjacency_matrix");
	report_routing_table = ReadParam<bool>(config, "report_routing_table");
	report_routes_stats = ReadParam<bool>(config, "report_routes_stats");
	report_possible_routes = ReadParam<bool>(config, "report_possible_routes");
	report_topology_sub_graph = ReadParam<bool>(config, "report_topology_sub_graph");
	report_topology_sub_graph_adjacency_matrix = ReadParam<bool>(config, "report_topology_sub_graph_adjacency_matrix");
	report_sub_routing_table = ReadParam<bool>(config, "report_sub_routing_table");
	report_cycle_result = ReadParam<bool>(config, "report_cycle_result");

	flit_trace_start = -1;
	flit_trace_end = -1;
	auto node = config["report_flit_trace"];
	if (node.IsSequence()) {
		report_flit_trace = true;
		flit_trace_start = node[0].as<std::int32_t>();
		if (node.size() == 2) {
			flit_trace_end = node[1].as<std::int32_t>();
		}
	} else {
		report_flit_trace = node.as<bool>();
	}

	clock_period_ps = ReadParam<std::int32_t>(config, "clock_period_ps");
	if (clock_period_ps < 1) {
		throw std::runtime_error("clock_period_ps can not be less than 1.");
	}
	reset_time = ReadParam<std::int32_t>(config, "reset_time");
	if (reset_time < 1) {
		throw std::runtime_error("reset_time can not be less than 1.");
	}
	simulation_time = ReadParam<std::int32_t>(config, "simulation_time");
	if (simulation_time < 1) {
		throw std::runtime_error("simulation_time can not be less than 1.");
	}
	stats_warm_up_time = ReadParam<std::int32_t>(config, "stats_warm_up_time");
	if (stats_warm_up_time < 0) {
		throw std::runtime_error("stats_warm_up_time can not be less than 0.");
	}

	min_packet_size = ReadParam<std::int32_t>(config, "min_packet_size");
	if (min_packet_size < 1) {
		throw std::runtime_error("min_packet_size can not be less than 1.");
	}
	max_packet_size = ReadParam<std::int32_t>(config, "max_packet_size");
	if (max_packet_size < 1) {
		throw std::runtime_error("max_packet_size can not be less than 1.");
	}
	flit_injection_rate = ReadParam<bool>(config, "flit_injection_rate");
	if (flit_injection_rate < 0) {
		throw std::runtime_error("flit_injection_rate can not be less than 0.");
	}
	scale_with_nodes = ReadParam<bool>(config, "scale_with_nodes");
	packet_injection_rate = ReadParam<double>(config, "packet_injection_rate");
	if (packet_injection_rate < 0) {
		throw std::runtime_error("packet_injection_rate can not be less than 0.");
	}


	if (max_packet_size < min_packet_size) {
		throw std::runtime_error("max_packet_size can not be less than min_packet_size.");
	}
}
void Configuration::ReadTrafficDistributionParams(const YAML::Node& config) {

	traffic_distribution = ReadParam<std::string>(config, "traffic_distribution");
	if (traffic_distribution == "TRAFFIC_TABLE_BASED")
		traffic_table_filename = ReadParam<std::string>(config, "traffic_table_filename");
	else if (traffic_distribution == "TRAFFIC_HOTSPOT") {
		const auto& traffic_hotspots = config["traffic_hotspots"];
		for (std::int32_t i = 0; i < traffic_hotspots.size(); i++) {
			const auto& hotspot = traffic_hotspots[i];
			hotspots.push_back(std::make_pair(
				hotspot[0].as<std::int32_t>(),
				std::make_pair(
					hotspot[1].as<std::int32_t>(), 
					hotspot[2].as<std::int32_t>()
				)
			));
		}
	}
}

void Configuration::ReportData() {
	if (report_topology_graph) {
		std::cout << "Topology graph: " << graph;
	}
	if (report_topology_graph_adjacency_matrix) {
		std::cout << "Topology graph adjacency matrix:\n" << graph.adjacency_matrix();
	}
	if (report_routing_table) {
		std::cout << "Routing table: " << table << '\n';
	}
	if (report_topology_sub_graph) {
		std::cout << "Topology subgraph: " << subgraph;
	}
	if (report_topology_sub_graph_adjacency_matrix) {
		std::cout << "Topology subgraph adjacency matrix:\n" << subgraph.adjacency_matrix();
	}
	if (report_sub_routing_table) {
		std::cout << "Subrouting table: " << subtable << '\n';
	}
	if (report_possible_routes) {
		std::cout << "Possible routes:\n";
		for (std::int32_t s = 0; s < graph.size(); s++) {
			for (std::int32_t d = 0; d < graph.size(); d++) {
				auto routes = table.GetPaths(graph, s, d);
				std::cout << std::setfill('0') << "[" << std::setw(2) << s << " -> " << std::setw(2) << d << "]:\n";
				for (std::int32_t i = 0; i < static_cast<std::int32_t>(routes.size()); i++) {
					const auto& route = routes[i];
					for (std::int32_t j = 0; j < static_cast<std::int32_t>(route.size()) - 1; j++) {
						std::cout << route[j] << " -> ";
					}
					if (route.size()) {
						std::cout << route.back();
					}
					std::cout << '\n';
				}
			}
		}
	}
	if (report_routes_stats) {
		std::size_t total_routes = 0;
		std::size_t total_distance = 0;
		std::size_t total_combinations = 0;
		std::size_t total_min_distance = 0;
		for (std::int32_t s = 0; s < graph.size(); s++) {
			for (std::int32_t d = 0; d < graph.size(); d++) {
				auto routes = table.GetPaths(graph, s, d);
				total_routes += routes.size();
				total_combinations++;
				std::size_t min_size = routes.front().size() - 1;
				for (const auto& route : routes) {
					total_distance += route.size() - 1;
					if (route.size() - 1 < min_size) {
						min_size = route.size() - 1;
					}
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
}

void Configuration::ShowHelp() {
	std::cout
		<< "Usage:\n"
		<< "  -parameter_name parameter_value\n";
}

Configuration::Configuration(std::int32_t arg_num, char* arg_vet[]) {
	// TODO: Maybe I should make it beautiful... But later

	for (std::int32_t i = 1; i < arg_num; i++) {
		if (!strcmp(arg_vet[i], "-help")) {
			ShowHelp();
			exit(0);
		}
	}

	YAML::Node config;
	std::string config_filename = default_config_filename;
	for (std::int32_t i = 1; i < arg_num; i++) {
		if (!strcmp(arg_vet[i], "-config")) {
			config_filename = arg_vet[++i];
			break;
		}
	}
	
	std::cout << "Loading configuration from file \"" << config_filename << "\"...";
	try {
		config = YAML::LoadFile(config_filename);
		std::cout << " Done\n";
	} catch (YAML::BadFile& e) {
		std::cout << " Failed\n";
		throw std::runtime_error("The specified YAML configuration file was not found!\nUse -config to load examples from config_examples folder.\n");
	} catch (YAML::ParserException& pe) {
		std::cout << " Failed\n";
		throw std::runtime_error((std::stringstream()
			<< "ERROR at line ["
			<< pe.mark.line + 1
			<< "] column ["
			<< pe.mark.column + 1
			<< "]: "
			<< pe.msg
			<< "."
		).str());
	}


	ParseArgs(config, arg_num, arg_vet);


	ReadTopologyParams(config);
	ReadRouterParams(config);
	ReadRoutingTableParams(config);
	ReadSimulationParams(config);
	ReadTrafficDistributionParams(config);

	ReportData();
}
void Configuration::ParseArgs(YAML::Node& node, std::int32_t arg_num, char* arg_vet[]) {
	for (std::int32_t i = 1; i < arg_num; i++) {
		if (arg_vet[i][0] == '-') {
			if (i + 1 >= arg_num) {
				throw std::runtime_error((std::stringstream()
					<< "Option ["
					<< arg_vet[i]
					<< "] requires an argument."
				).str());
			}

			char* key = arg_vet[i] + 1;
			char* value = arg_vet[++i];
			if (value[0] != '[') {
				node[key] = value;
			} else {
				node[key] = YAML::Node(YAML::NodeType::Sequence);
				std::string item;
				char *c = value + 1;
				while (*c != '\0') {
					if (std::isspace(*c) || *c == ',' || *c == ']') {
						if (!item.empty()) {
							node[key].push_back(item);
						}
						item.clear();
					} else {
						item += *c;
					}
					c++;
				}
			}
		} else {
			throw std::runtime_error((std::stringstream()
				<< "Invalid option ["
				<< arg_vet[i]
				<< "]."
			).str());
		}
	}
}

const Graph& Configuration::TopologyGraph() const {
	return graph;
}
const Graph& Configuration::TopologySubGraph() const {
	return subgraph;
}
const Graph& Configuration::NetworkGraph() const {
	return network_graph;
}
const RoutingTable& Configuration::GRTable() const {
	return table;
}
const RoutingTable& Configuration::SubGRTable() const {
	return subtable;
}
const std::vector<std::pair<std::int32_t, std::pair<std::int32_t, std::int32_t>>>& Configuration::Hotspots() const {
	return hotspots;
}

const std::vector<std::int32_t> Configuration::UpdateSequence() const {
	return update_sequence;
}
std::int32_t Configuration::BufferDepth() const {
	return buffer_depth;
}
std::int32_t Configuration::MinPacketSize() const {
	return min_packet_size;
}
std::int32_t Configuration::MaxPacketSize() const {
	return max_packet_size;
}
const std::string& Configuration::RoutingAlgorithm() const {
	return routing_algorithm;
}
const std::string& Configuration::SelectionStrategy() const {
	return selection_strategy;
}
double Configuration::PacketInjectionRate() const {
	if (!flit_injection_rate) return packet_injection_rate / (scale_with_nodes ? graph.size() : 1.0);
	else return packet_injection_rate * 2.0 / (min_packet_size + max_packet_size) / (scale_with_nodes ? graph.size() : 1.0);
}
double Configuration::Locality() const {
	return locality;
}
const std::string& Configuration::TrafficDistribution() const {
	return traffic_distribution;
}
const std::string& Configuration::TrafficTableFilename() const {
	return traffic_table_filename;
}
std::int32_t Configuration::ClockPeriodPS() const {
	return clock_period_ps;
}
std::int32_t Configuration::SimulationTime() const {
	return simulation_time;
}
std::int32_t Configuration::ResetTime() const {
	return reset_time;
}
std::int32_t Configuration::StatsWarmUpTime() const {
	return stats_warm_up_time;
}
std::int32_t Configuration::RndGeneratorSeed() const {
	return rnd_generator_seed;
}
bool Configuration::ReportProgress() const {
	return report_progress;
}
bool Configuration::JsonResult() const {
	return json_result;
}
bool Configuration::ReportBuffers() const {
	return report_buffers;
}
bool Configuration::ReportCycleResult() const {
	return report_cycle_result;
}
bool Configuration::ReportFlitTrace() const {
	return report_flit_trace;
}
double Configuration::FlitTraceStart() const {
	return flit_trace_start;
}
double Configuration::FlitTraceEnd() const {
	return flit_trace_end;
}

std::int32_t Configuration::DimX() const {
	return dim_x;
}
std::int32_t Configuration::DimY() const {
	return dim_y;
}
std::int32_t Configuration::ChannelsCount() const {
	return channels_count;
}
std::size_t Configuration::VirtualChannels() const {
	return virtual_channels_count;
}

double Configuration::TimeStamp() const {
	return sc_time_stamp().to_double() / clock_period_ps - reset_time;
}
