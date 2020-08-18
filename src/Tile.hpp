/*
 * Noxim - the NoC Simulator
 *
 * (C) 2005-2018 by the University of Catania
 * For the complete list of authors refer to file ../doc/AUTHORS.txt
 * For the license applied to these sources refer to file ../doc/LICENSE.txt
 *
 * This file contains the declaration of the tile
 */

#pragma once
#include <systemc.h>
#include "Router.hpp"
#include "Processor.hpp"



class Tile : public sc_module
{
public:
	// I/O Ports
	sc_in_clk clock;							// The input clock for the tile
	sc_in<bool> reset;	                        // The reset signal for the tile

	// Instances
	std::unique_ptr<Router> RouterDevice;
	std::unique_ptr<Processor> ProcessorDevice;
		
	// Constructor
	Tile(sc_module_name nm)
	{
	}
	void SetRouter(std::unique_ptr<Router>& router)
	{
		if (!router) throw std::runtime_error("Tile error: Router can not be null.");

		RouterDevice = std::move(router);
		// Router pin assignments
		RouterDevice->clock(clock);
		RouterDevice->reset(reset);
	}
	void SetProcessor(std::unique_ptr<Processor>& processor)
	{
		if (!processor) throw std::runtime_error("Tile error: Processor can not be null.");

		ProcessorDevice = std::move(processor);

		// Processing Element pin assignments
		ProcessorDevice->clock(clock);
		ProcessorDevice->reset(reset);
	}

	void ConfigureRotuerPower(const Configuration::Power& config, std::int32_t flit_size, std::int32_t buffer_depth, const std::string& routing_algorithm)
	{
		RouterDevice->power.configureRouter(config, 
			flit_size, buffer_depth, flit_size,
			routing_algorithm, "default");
	}
};
