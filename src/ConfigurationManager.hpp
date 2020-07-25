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

#ifndef __NOXIMCONFIGURATIONMANAGER_H__
#define __NOXIMCONFIGURATIONMANAGER_H__

#include "yaml-cpp/yaml.h"
#include "GlobalParams.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <utility>



void configure(int arg_num, char* arg_vet[]);

template <typename T>
T readParam(YAML::Node node, std::string param, T default_value);

template <typename T>
T readParam(YAML::Node node, std::string param);

namespace YAML 
{
	template<>
	struct convert<BufferPowerConfig> {
		static bool decode(const Node& node, BufferPowerConfig& bufferPowerConfig) {
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
	struct convert<LinkBitLinePowerConfig> {
		static bool decode(const Node& node, LinkBitLinePowerConfig& linkBitLinePowerConfig) {
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
	struct convert<RouterPowerConfig> {
		static bool decode(const Node& node, RouterPowerConfig& routerPowerConfig) {

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
	struct convert<PowerConfig> {
		static bool decode(const Node& node, PowerConfig& powerConfig) {
			powerConfig.bufferPowerConfig = node["Buffer"].as<BufferPowerConfig>();
			powerConfig.linkBitLinePowerConfig = node["LinkBitLine"].as<LinkBitLinePowerConfig>();
			powerConfig.routerPowerConfig = node["Router"].as<RouterPowerConfig>();
			return true;
		}
	};
}
#endif
