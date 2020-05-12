#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <map>
#include "Graph.h"
#include "RoutingTable.h"



class Configuration
{
public:
	struct BufferPower
	{
		std::map<std::pair<int32_t, int32_t>, double> front;
		std::map<std::pair<int32_t, int32_t>, double> pop;
		std::map<std::pair<int32_t, int32_t>, double> push;
		std::map<std::pair<int32_t, int32_t>, double> leakage;
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
	};

private:
	std::string topology;
	std::string topology_args;

	std::string verbose_mode;
	double r2r_link_length;
	int32_t buffer_depth;
	int32_t flit_size;
	int32_t min_packet_size;
	int32_t max_packet_size;
	std::string routing_algorithm;
	std::string selection_strategy;
	double packet_injection_rate;
	double probability_of_retransmission;
	double locality;
	std::string traffic_distribution;
	std::string traffic_table_filename;
	std::string config_filename;
	std::string power_config_filename;
	int32_t clock_period_ps;
	int32_t simulation_time;
	int32_t reset_time;
	int32_t stats_warm_up_time;
	int32_t rnd_generator_seed;
	double dyad_threshold;
	uint32_t max_volume_to_be_drained;
	Power power_configuration;
	int32_t dim_x, dim_y;
	int32_t channels_count;

	Graph graph;
	RoutingTable table;

public:
	static std::string default_config_filename;
	static std::string default_power_config_filename;

	static void ShowHelp(const std::string& selfname);

	Configuration(int32_t arg_num, char* arg_vet[]);
	void ParseArgs(int32_t arg_num, char* arg_vet[]);
	void Check();

	void Show() const;

	const Graph& Topology() const;
	const RoutingTable& GRTable() const;

	const std::string& VerboseMode() const;
	double R2RLinkLength() const;
	int32_t BufferDepth() const;
	int32_t FlitSize() const;
	int32_t MinPacketSize() const;
	int32_t MaxPacketSize() const;

	const std::string& RoutingAlgorithm() const;
	const std::string& SelectionStrategy() const;
	double PacketInjectionRate() const;
	double ProbabilityOfRetransmission() const;
	double Locality() const;
	const std::string& TrafficDistribution() const;
	const std::string& TrafficTableFilename() const;
	const std::string& ConfigFilename() const;
	const std::string& PowerConfigFilename() const;
	int32_t ClockPeriodPS() const;
	int32_t SimulationTime() const;
	int32_t ResetTime() const;
	int32_t StatsWarmUpTime() const;
	int32_t RndGeneratorSeed() const;
	std::vector<std::pair<int, double>> Hotspots() const;
	double DyadThreshold() const;
	uint32_t MaxVolumeToBeDrained() const;
	const Power& PowerConfiguration() const;

	int32_t DimX() const;
	int32_t DimY() const;
	int32_t ChannelsCount() const;

	double TimeStamp() const;
};

