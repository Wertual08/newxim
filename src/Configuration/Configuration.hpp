#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "Graph/Graph.hpp"
#include "RoutingTable.hpp"
#include "TrafficManagers/TrafficManager.hpp"



namespace YAML
{
	class Node;
}
class Configuration
{
private:
	std::string topology;
	std::string topology_args;

	std::vector<std::int32_t> update_sequence;
	std::int32_t buffer_depth;
	std::int32_t min_packet_size;
	std::int32_t max_packet_size;
	std::string routing_algorithm;
	std::string selection_strategy;
	bool flit_injection_rate;
	bool scale_with_nodes;
	double packet_injection_rate;
	double probability_of_retransmission;
	double locality;
	std::string traffic_distribution;
	std::string traffic_table_filename;
	std::int32_t clock_period_ps;
	std::int32_t simulation_time;
	std::int32_t reset_time;
	std::int32_t stats_warm_up_time;
	std::int32_t rnd_generator_seed;
	std::int32_t dim_x, dim_y;
	std::int32_t channels_count;
	std::size_t virtual_channels_count;
	bool report_progress;
	bool json_result;
	bool report_buffers;
	bool report_routing_table;
	bool report_possible_routes;
	bool report_routes_stats;
	bool report_topology_graph;
	bool report_topology_graph_adjacency_matrix;
	bool report_sub_routing_table;
	bool report_topology_sub_graph;
	bool report_topology_sub_graph_adjacency_matrix;
	bool report_cycle_result;
	bool report_flit_trace;
	double flit_trace_start;
	double flit_trace_end;

	std::vector<std::pair<std::int32_t, std::pair<std::int32_t, std::int32_t>>> hotspots;

	Graph graph;
	Graph subgraph;
	Graph network_graph;
	RoutingTable table;
	RoutingTable subtable;

	void ReadTopologyParams(const YAML::Node& config);
	void ReadRouterParams(const YAML::Node& config);
	void ReadRoutingTableParams(const YAML::Node& config);
	void ReadSimulationParams(const YAML::Node& config);
	void ReadTrafficDistributionParams(const YAML::Node& config);

	void ReportData();

public:
	static std::string default_config_filename;

	static void ShowHelp();

	Configuration(std::int32_t arg_num, char* arg_vet[]);
	void ParseArgs(YAML::Node &node, std::int32_t arg_num, char* arg_vet[]);

	const Graph &TopologyGraph() const;
	const Graph &TopologySubGraph() const;
	const Graph &NetworkGraph() const;
	const RoutingTable& GRTable() const;
	const RoutingTable& SubGRTable() const;
	const std::vector<std::pair<std::int32_t, std::pair<std::int32_t, std::int32_t>>>& Hotspots() const;

	const std::vector<std::int32_t> UpdateSequence() const;
	std::int32_t BufferDepth() const;
	std::int32_t MinPacketSize() const;
	std::int32_t MaxPacketSize() const;

	const std::string& RoutingAlgorithm() const;
	const std::string& SelectionStrategy() const;
	double PacketInjectionRate() const;
	double ProbabilityOfRetransmission() const;
	double Locality() const;
	const std::string& TrafficDistribution() const;
	const std::string& TrafficTableFilename() const;
	std::int32_t ClockPeriodPS() const;
	std::int32_t SimulationTime() const;
	std::int32_t ResetTime() const;
	std::int32_t StatsWarmUpTime() const;
	std::int32_t RndGeneratorSeed() const;
	bool ReportProgress() const;
	bool JsonResult() const;
	bool ReportBuffers() const;
	bool ReportCycleResult() const;
	bool ReportFlitTrace() const;
	double FlitTraceStart() const;
	double FlitTraceEnd() const;

	std::int32_t DimX() const;
	std::int32_t DimY() const;
	std::int32_t ChannelsCount() const;
	std::size_t VirtualChannels() const;

	double TimeStamp() const;
};

