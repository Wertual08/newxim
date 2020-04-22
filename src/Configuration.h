#pragma once
#include <cstdint>
#include <vector>
#include <string>
#include <map>



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
	std::string verbose_mode;
	int32_t trace_mode;
	std::string trace_filename;
	double r2r_link_length;
	int32_t buffer_depth;
	int32_t flit_size;
	int32_t min_packet_size;
	int32_t max_packet_size;
	std::string routing_algorithm;
	std::string routing_table_filename;
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
	int32_t n_virtual_channels;
	int32_t reset_time;
	int32_t stats_warm_up_time;
	int32_t rnd_generator_seed;
	bool detailed;
	std::vector<std::pair<int, double>> hotspots;
	double dyad_threshold;
	uint32_t max_volume_to_be_drained;
	bool show_buffer_stats;
	bool use_powermanager;
	Power power_configuration;

public:
	static constexpr const char* VERBOSE_OFF = "VERBOSE_OFF";
	static constexpr const char* VERBOSE_LOW = "VERBOSE_LOW";
	static constexpr const char* VERBOSE_MEDIUM = "VERBOSE_MEDIUM";
	static constexpr const char* VERBOSE_HIGH = "VERBOSE_HIGH";
	static constexpr const char* ROUTING_TABLE_BASED = "TABLE_BASED";
	static constexpr const char* TRAFFIC_RANDOM = "TRAFFIC_RANDOM";
	static constexpr const char* TRAFFIC_TABLE_BASED = "TRAFFIC_TABLE_BASED";

	static std::string default_config_filename;
	static std::string default_power_config_filename;

	static void ShowHelp(const std::string& selfname);

	Configuration(int32_t arg_num, char* arg_vet[]);
	void ParseArgs(int32_t arg_num, char* arg_vet[]);
	void Check();

	void Show() const;

	const std::string& VerboseMode() const;
	int32_t TraceMode() const;
	const std::string& TraceFilename() const;
	double R2RLinkLength() const;
	int32_t BufferDepth() const;
	int32_t FlitSize() const;
	int32_t MinPacketSize() const;
	int32_t MaxPacketSize() const;
	const std::string& RoutingAlgorithm() const;
	const std::string& RoutingTableFilename() const;
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
	int32_t NVirtualChannels() const;
	int32_t ResetTime() const;
	int32_t StatsWarmUpTime() const;
	int32_t RndGeneratorSeed() const;
	bool Detailed() const;
	std::vector<std::pair<int, double>> Hotspots() const;
	double DyadThreshold() const;
	uint32_t MaxVolumeToBeDrained() const;
	bool ShowBufferStats() const;
	bool UsePowermanager() const;
	const Power& PowerConfiguration() const;

	double TimeStamp() const;
};

