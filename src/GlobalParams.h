/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the global params needed by Noxim
 * to forward configuration to every sub-block
 */

#pragma once
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <cassert>

#define CONFIG_FILENAME        "config.yaml"
#define POWER_CONFIG_FILENAME  "power.yaml"

// Define the directions as numbers
#define MAX_VIRTUAL_CHANNELS	8
#define DEFAULT_VC 		0

#define RT_AVAILABLE 1
#define RT_ALREADY_SAME -1
#define RT_ALREADY_OTHER_OUT -2
#define RT_OUTVC_BUSY -3

// To mark invalid or non exhistent values
#define NOT_VALID             -1

// Routing algorithms
#define ROUTING_DYAD           "DYAD"
#define ROUTING_TABLE_BASED    "TABLE_BASED"

// Traffic distribution
#define TRAFFIC_RANDOM         "TRAFFIC_RANDOM"
#define TRAFFIC_TRANSPOSE1     "TRAFFIC_TRANSPOSE1"
#define TRAFFIC_TRANSPOSE2     "TRAFFIC_TRANSPOSE2"
#define TRAFFIC_HOTSPOT        "TRAFFIC_HOTSPOT"
#define TRAFFIC_TABLE_BASED    "TRAFFIC_TABLE_BASED"
#define TRAFFIC_BIT_REVERSAL   "TRAFFIC_BIT_REVERSAL"
#define TRAFFIC_SHUFFLE        "TRAFFIC_SHUFFLE"
#define TRAFFIC_BUTTERFLY      "TRAFFIC_BUTTERFLY"
#define TRAFFIC_LOCAL	       "TRAFFIC_LOCAL"
#define TRAFFIC_ULOCAL	       "TRAFFIC_ULOCAL"

// Verbosity levels
#define VERBOSE_OFF            "VERBOSE_OFF"
#define VERBOSE_LOW            "VERBOSE_LOW"
#define VERBOSE_MEDIUM         "VERBOSE_MEDIUM"
#define VERBOSE_HIGH           "VERBOSE_HIGH"

struct BufferPowerConfig 
{
	std::map<std::pair<int, int>, double> front;
	std::map<std::pair<int, int>, double> pop;
	std::map<std::pair<int, int>, double> push;
	std::map<std::pair<int, int>, double> leakage;
};

typedef std::map<double, std::pair<double, double>> LinkBitLinePowerConfig;

struct RouterPowerConfig 
{
	std::map<std::pair<double, double>, std::pair<double, double> > crossbar_pm;
	std::map<int, std::pair<double, double> > network_interface;
	std::map<std::string, std::pair<double, double> > routing_algorithm_pm;
	std::map<std::string, std::pair<double, double> > selection_strategy_pm;
};

struct PowerConfig 
{
	BufferPowerConfig bufferPowerConfig;
	LinkBitLinePowerConfig linkBitLinePowerConfig;
	RouterPowerConfig routerPowerConfig;
};

struct GlobalParams 
{
	static std::string verbose_mode;
	static double r2r_link_length;
	static int buffer_depth;
	static int flit_size;
	static int min_packet_size;
	static int max_packet_size;
	static double packet_injection_rate;
	static double probability_of_retransmission;
	static double locality;
	static std::string traffic_distribution;
	static std::string config_filename;
	static std::string power_config_filename;
	static int clock_period_ps;
	static int simulation_time;
	static int reset_time;
	static int stats_warm_up_time;
	static double dyad_threshold;
	static unsigned int max_volume_to_be_drained;
	static PowerConfig power_configuration;
};