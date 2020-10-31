/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the implementation of the power model
 */

#include <iostream>
#include "Power.hpp"
#include "Utils.hpp"
#include "systemc.h"



double Power::W2J(double watt)
{
    return watt * Timer.ClockPeriod() * 1.0e-12;
}

Power::Power(const SimulationTimer& timer) : Timer(timer)
{
    total_power_s = 0.0;

    buffer_router_push_pwr_d = 0.0;
    buffer_router_pop_pwr_d = 0.0;
    buffer_router_front_pwr_d = 0.0;
    buffer_router_pwr_s = 0.0;
    
    buffer_to_tile_push_pwr_d = 0.0;
    buffer_to_tile_pop_pwr_d = 0.0;
    buffer_to_tile_front_pwr_d = 0.0;
    buffer_to_tile_pwr_s = 0.0;

    buffer_from_tile_push_pwr_d = 0.0;
    buffer_from_tile_pop_pwr_d = 0.0;
    buffer_from_tile_front_pwr_d = 0.0;
    buffer_from_tile_pwr_s = 0.0;

    routing_pwr_d = 0.0;
    routing_pwr_s = 0.0;

    selection_pwr_d = 0.0;
    selection_pwr_s = 0.0;

    crossbar_pwr_d = 0.0;
    crossbar_pwr_s = 0.0;

    link_r2r_pwr_d = 0.0;
    link_r2r_pwr_s = 0.0;

    default_tx_energy = 0.0;

    ni_pwr_d = 0.0;
    ni_pwr_s = 0.0;


    sleep_end_cycle = -1; // NOT_VALID

    initPowerBreakdown();
}

void Power::configureRouter(
    const Configuration::Power& config_, 
    int link_width,
	int buffer_depth,
	int buffer_item_size,
	std::string routing_function,
	std::string selection_function)
{
    Configuration::Power config = config_; // TODO: Fix this shit
// (s)tatic, (d)ynamic power

    // Buffer 
    std::pair<int, int> key = std::pair<int, int>(buffer_depth, buffer_item_size);

    // Dynamic values are expressed in Joule
    // Static/Leakage values must be converted from Watt to Joule

    buffer_router_pwr_s = W2J(config.bufferPowerConfig.leakage[key]);
    buffer_router_push_pwr_d = config.bufferPowerConfig.push[key];
    buffer_router_front_pwr_d = config.bufferPowerConfig.front[key];
    buffer_router_pop_pwr_d = config.bufferPowerConfig.pop[key];

    // Routing 
    routing_pwr_s = W2J(config.routerPowerConfig.routing_algorithm_pm[routing_function].first);
    routing_pwr_d = config.routerPowerConfig.routing_algorithm_pm[routing_function].second;

    // Selection 
    selection_pwr_s = W2J(config.routerPowerConfig.selection_strategy_pm[selection_function].first);
    selection_pwr_d = config.routerPowerConfig.selection_strategy_pm[selection_function].second;

    // CrossBar
    // TODO future work: tuning of crossbar radix
    std::pair<int,int> xbar_k = std::pair<int,int>(5, buffer_item_size);
    assert(config.routerPowerConfig.crossbar_pm.find(xbar_k) != config.routerPowerConfig.crossbar_pm.end());
    crossbar_pwr_s = W2J(config.routerPowerConfig.crossbar_pm[xbar_k].first);
    crossbar_pwr_d = config.routerPowerConfig.crossbar_pm[xbar_k].second;
    
    // NetworkInterface
    ni_pwr_s = W2J(config.routerPowerConfig.network_interface[buffer_item_size].first);
    ni_pwr_d = config.routerPowerConfig.network_interface[buffer_item_size].second;

    // Link 
    // Router has both type of links
    double length_r2r = config.r2r_link_length;
    
    assert(config.linkBitLinePowerConfig.find(length_r2r)!=config.linkBitLinePowerConfig.end());


    link_r2r_pwr_s= W2J(link_width * config.linkBitLinePowerConfig[length_r2r].first);
    link_r2r_pwr_d= link_width * config.linkBitLinePowerConfig[length_r2r].second;
}

// Router buffer
void Power::bufferRouterPush()
{
    power_dynamic.breakdown[BUFFER_PUSH_PWR_D].value += buffer_router_push_pwr_d;
}

void Power::bufferRouterPop()
{
    power_dynamic.breakdown[BUFFER_POP_PWR_D].value += buffer_router_pop_pwr_d;
}

void Power::bufferRouterFront()
{
    power_dynamic.breakdown[BUFFER_FRONT_PWR_D].value += buffer_router_front_pwr_d;
}

void Power::routing()
{
    power_dynamic.breakdown[ROUTING_PWR_D].value += routing_pwr_d;
}

void Power::selection()
{
    power_dynamic.breakdown[SELECTION_PWR_D].value +=selection_pwr_d ;
}

void Power::crossBar()
{
    power_dynamic.breakdown[CROSSBAR_PWR_D].value +=crossbar_pwr_d;
}

void Power::r2rLink()
{
    power_dynamic.breakdown[LINK_R2R_PWR_D].value +=link_r2r_pwr_d;
}
void Power::networkInterface()
{
    power_dynamic.breakdown[NI_PWR_D].value +=ni_pwr_d;
}


double Power::getDynamicPower()
{
    double power = 0.0;
    for (int i = 0; i<power_dynamic.size; i++)
    {
	power+= power_dynamic.breakdown[i].value;
    }

    return power;
}

double Power::getStaticPower()
{
    double power = 0.0;
    for (int i = 0; i<power_static.size; i++)
	power+= power_static.breakdown[i].value;

    return power;
}


double Power::attenuation2power(double attenuation)
{
    // TODO
    return attenuation;
}

// Note: In the following 3 functions buffer_pwr_s 
// is assumed as loaded with the proper values from configuration file:
// - Router: takes the value of input buffers leakage
void Power::leakageBufferRouter()
{
    power_static.breakdown[BUFFER_ROUTER_PWR_S].value +=buffer_router_pwr_s;
}

void Power::leakageBufferToTile()
{
    power_static.breakdown[BUFFER_TO_TILE_PWR_S].value +=buffer_to_tile_pwr_s;
}

void Power::leakageBufferFromTile()
{
    power_static.breakdown[BUFFER_FROM_TILE_PWR_S].value +=buffer_from_tile_pwr_s;
}

void Power::leakageLinkRouter2Router()
{
    //power_static.breakdown[LINK_R2R_PWR_S].value +=link_r2r_pwr_s;
}

void Power::leakageRouter()
{
    // note: leakage contributions depending on instance number are 
    // accounted in specific separate leakage functions
    power_static.breakdown[ROUTING_PWR_S].value +=routing_pwr_s;
    power_static.breakdown[SELECTION_PWR_S].value +=selection_pwr_s;
    power_static.breakdown[CROSSBAR_PWR_S].value +=crossbar_pwr_s;
    power_static.breakdown[NI_PWR_S].value +=ni_pwr_s;
}

void Power::printBreakDown(std::ostream & out)
{
    assert(false);
    //printMap("power_dynamic",power_dynamic,cout);
    //printMap("power_static",power_static,cout);
}


void Power::rxSleep(int cycles)
{

    int sleep_start_cycle = static_cast<std::int32_t>(Timer.SystemTime());
    sleep_end_cycle = sleep_start_cycle + cycles;
}


bool Power::isSleeping()
{
    int now = static_cast<std::int32_t>(Timer.SystemTime());

    return (now<sleep_end_cycle);

}


void Power::initPowerBreakdownEntry(PowerBreakdownEntry* pbe, std::string label)
{
    pbe->label = label;
    pbe->value = 0.0;
}



void Power::initPowerBreakdown()
{
    power_dynamic.size = NO_BREAKDOWN_ENTRIES_D;
    power_static.size = NO_BREAKDOWN_ENTRIES_S;

    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_PUSH_PWR_D], "buffer_push_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_POP_PWR_D],"buffer_pop_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_FRONT_PWR_D],"buffer_front_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_TO_TILE_PUSH_PWR_D],"buffer_to_tile_push_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_TO_TILE_POP_PWR_D],"buffer_to_tile_pop_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_TO_TILE_FRONT_PWR_D],"buffer_to_tile_front_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_FROM_TILE_PUSH_PWR_D],"buffer_from_tile_push_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_FROM_TILE_POP_PWR_D],"buffer_from_tile_pop_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[BUFFER_FROM_TILE_FRONT_PWR_D],"buffer_from_tile_front_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[ANTENNA_BUFFER_PUSH_PWR_D],"antenna_buffer_push_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[ANTENNA_BUFFER_POP_PWR_D],"antenna_buffer_pop_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[ANTENNA_BUFFER_FRONT_PWR_D],"antenna_buffer_front_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[ROUTING_PWR_D],"routing_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[SELECTION_PWR_D],"selection_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[CROSSBAR_PWR_D],"crossbar_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[LINK_R2R_PWR_D],"link_r2r_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[LINK_R2H_PWR_D],"link_r2h_pwr_d");
    initPowerBreakdownEntry(&power_dynamic.breakdown[NI_PWR_D],"ni_pwr_d");

    initPowerBreakdownEntry(&power_static.breakdown[TRANSCEIVER_RX_PWR_BIASING],"transceiver_rx_pwr_biasing");
    initPowerBreakdownEntry(&power_static.breakdown[TRANSCEIVER_TX_PWR_BIASING],"transceiver_tx_pwr_biasing");
    initPowerBreakdownEntry(&power_static.breakdown[BUFFER_ROUTER_PWR_S],"buffer_router_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[BUFFER_TO_TILE_PWR_S],"buffer_to_tile_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[BUFFER_FROM_TILE_PWR_S],"buffer_from_tile_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[ANTENNA_BUFFER_PWR_S],"antenna_buffer_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[LINK_R2H_PWR_S],"link_r2h_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[ROUTING_PWR_S],"routing_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[SELECTION_PWR_S],"selection_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[CROSSBAR_PWR_S],"crossbar_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[NI_PWR_S],"ni_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[TRANSCEIVER_RX_PWR_S],"transceiver_rx_pwr_s");
    initPowerBreakdownEntry(&power_static.breakdown[TRANSCEIVER_TX_PWR_S],"transceiver_tx_pwr_s");
}

    



