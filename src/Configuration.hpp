#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "Graph.hpp"
#include "RoutingTable.hpp"
#include "TrafficManager.hpp"



class Configuration
{
public:
	struct BufferPower
	{
		std::map<std::pair<std::int32_t, std::int32_t>, double> front;
		std::map<std::pair<std::int32_t, std::int32_t>, double> pop;
		std::map<std::pair<std::int32_t, std::int32_t>, double> push;
		std::map<std::pair<std::int32_t, std::int32_t>, double> leakage;
	};

	struct RouterPower
	{
		std::map<std::pair<double, double>, std::pair<double, double> > crossbar_pm;
		std::map<int, std::pair<double, double> > network_interface;
		std::map<std::string, std::pair<double, double> > routing_algorithm_pm;
		std::map<std::string, std::pair<double, double> > selection_strategy_pm;
	};

	struct Power
	{
		BufferPower bufferPowerConfig;
		std::map<double, std::pair<double, double>> linkBitLinePowerConfig;
		RouterPower routerPowerConfig;
		double r2r_link_length;
	};

private:
	std::string topology;
	std::string topology_args;

	std::int32_t buffer_depth;
	std::int32_t flit_size;
	std::int32_t min_packet_size;
	std::int32_t max_packet_size;
	std::string router_type;
	std::string routing_algorithm;
	std::string selection_strategy;
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
	Power power_configuration;
	std::int32_t dim_x, dim_y;
	std::int32_t channels_count;
	bool report_progress;
	bool report_buffers;
	bool report_routing_table;

	std::vector<std::pair<std::int32_t, std::pair<std::int32_t, std::int32_t>>> hotspots;

	Graph graph;
	RoutingTable table;

public:
	static std::string default_config_filename;
	static std::string default_power_config_filename;

	static void ShowHelp(const std::string& selfname);

	Configuration(std::int32_t arg_num, char* arg_vet[]);
	void ParseArgs(std::int32_t arg_num, char* arg_vet[]);
	void Check();

	void Show() const;

	const Graph& TopologyGraph() const;
	const RoutingTable& GRTable() const;
	const std::vector<std::pair<std::int32_t, std::pair<std::int32_t, std::int32_t>>>& Hotspots() const;

	std::int32_t BufferDepth() const;
	std::int32_t FlitSize() const;
	std::int32_t MinPacketSize() const;
	std::int32_t MaxPacketSize() const;

	const std::string& RouterType() const;
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
	const Power& PowerConfiguration() const;
	bool ReportProgress() const;
	bool ReportBuffers() const;
	bool ReportRoutingTable() const;

	std::int32_t DimX() const;
	std::int32_t DimY() const;
	std::int32_t ChannelsCount() const;

	double TimeStamp() const;
};

