/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the command line parser
 */

#include "GlobalParams.h"

double GlobalParams::r2r_link_length;
int GlobalParams::buffer_depth;
int GlobalParams::flit_size;
int GlobalParams::min_packet_size;
int GlobalParams::max_packet_size;
double GlobalParams::packet_injection_rate;
double GlobalParams::probability_of_retransmission;
double GlobalParams::locality;
std::string GlobalParams::traffic_distribution;
std::string GlobalParams::config_filename;
std::string GlobalParams::power_config_filename;
int GlobalParams::clock_period_ps;
int GlobalParams::simulation_time;
int GlobalParams::reset_time;
int GlobalParams::stats_warm_up_time;
double GlobalParams::dyad_threshold;
PowerConfig GlobalParams::power_configuration;
